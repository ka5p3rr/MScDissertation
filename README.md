# MSc Dissertation at the University of Edinburgh <!-- omit in toc -->

## Table of contents <!-- omit in toc -->

- [Compression](#compression)
  - [snappy-c](#snappy-c)
  - [zip](#zip)
  - [FastLZ](#fastlz)
  - [zlib](#zlib)
  - [QuickLZ](#quicklz)
    - [Decompression investigation and bugs](#decompression-investigation-and-bugs)
    - [Compression investigation](#compression-investigation)
  - [lzbench](#lzbench)
  - [minilzo](#minilzo)
  - [miniz](#miniz)
  - [liblzf](#liblzf)
- [ImageMagick](#imagemagick)
  - [Running instructions](#running-instructions)
  - [Porting](#porting)
  - [Issues](#issues)
    - [Testing results](#testing-results)
  - [Notes and links](#notes-and-links)
- [Miscellaneous porting](#miscellaneous-porting)

## Compression

### snappy-c

- Snappy-c is third-party implementation of the snappy algorithm providing C-language bindings
- Compiles on Morello purecap without issues, build using gmake. Resulting binaries are executable including a provided verifying script, command line interface and/or object files to be included in other projects.
- Running on Morello provides runtime safety, for example providing wrongly sized buffers results in a runtime error. Similarly, testing unaligned sizes with provided buffers, uninitialised memory and similar. Question is whether the code should check for the missmatch or just fail on Morello? I assume on traditional architecture it will fail as well with an alternative error, but the code is then open to certain vulnerabilities, presumably.
- Running verify (their testing file) shows no issues. I have written short bash script to handle running on compression-corpus, which contains several files of different formatting, file extensions and length to be tested.
- [snappy-c on GitHub](https://github.com/andikleen/snappy-c), [snappy-c fork](https://github.com/ka5p3rr/snappy-c), [snappy-c API](http://halobates.de/snappy.html), [Main project website](https://github.com/google/snappy), [snappy on GitHub](https://github.com/google/snappy)

### zip

- Library had a buffer overflow bug, which was discovered through their provided tests.

```bash
Running tests...
/usr/local64/bin/ctest --force-new-ctest-process 
Test project /usr/home/s1765150/dissertation/zip/build
    Start 1: test_write.out
1/6 Test #1: test_write.out ...................   Passed    0.00 sec
    Start 2: test_append.out
2/6 Test #2: test_append.out ..................   Passed    0.00 sec
    Start 3: test_read.out
3/6 Test #3: test_read.out ....................   Passed    0.00 sec
    Start 4: test_extract.out
4/6 Test #4: test_extract.out .................   Passed    0.00 sec
    Start 5: test_entry.out
5/6 Test #5: test_entry.out ...................Signal 34***Exception:   0.20 sec
    Start 6: test_permissions.out
6/6 Test #6: test_permissions.out .............   Passed    0.01 sec

83% tests passed, 1 tests failed out of 6

Total Test time (real) =   0.22 sec

The following tests FAILED:
          5 - test_entry.out (Signal 34)
Errors while running CTest
Output from these tests are in: /usr/home/s1765150/dissertation/zip/build/Testing/Temporary/LastTest.log
Use "--rerun-failed --output-on-failure" to re-run the failed cases verbosely.
gmake: *** [Makefile:74: test] Error 8
```

In summary, the code contained a check for an array index. The check is tested each loop iteration, but only after accessing the array already. Ultimately, resulting in buffer over-read of the last index, which is out of the bounds of the buffer.

Running the failing test individually results in a `In-address space security exception (core dumped)` error, which are caused by a flipped `&&` condition of an array index. More detail is provided in the issue filed in the original repository description here [issue](https://github.com/kuba--/zip/issues/270).

Ruize and I found the bug through different means, we updated each other on the issue and collaborated on finding a fix. The fix has now been upstreamed to the original repository. Testing the new release on Morello all tests now pass successfully.

- [zip](https://github.com/kuba--/zip), "This is done by hacking awesome [miniz](https://github.com/richgel999/miniz) library and layering functions on top of the miniz v2.2.0 API."

### FastLZ

- Found a bug, Ruize found a fix, which makes all the tests pass. A buffer overflow caused a `In-address space security exception (core dumped)`.
- Code alteration can be found in the submodule, which is a fork of the original repository. Bug fix included and added `defined(__aarch64__) || defined(_M_ARM64)` to recognise ARM as a 64-bit system.
- [FastLZ](https://github.com/ariya/FastLZ), [FastLZ fork](https://github.com/ka5p3rr/FastLZ)

### zlib

- Compiles with no issues, all tests pass
- [Main project website](http://zlib.net/), [zlib on GitHub](https://github.com/madler/zlib)

### QuickLZ

- Seems to cause a memory fault `In-address space security exception (core dumped)`, this happens only when decompressing with `QLZ_STREAMING_BUFFER 0` and `QLZ_COMPRESSION_LEVEL 1`
- The code contains a large amount of sections, which are either omitted or included in compilation based on several defines

#### Decompression investigation and bugs

An investigation shows a cast of `(const unsigned char *)(size_t)` from a `const unsigned char *` type, which causes the store pointer to not be dereferencable. A pointer variable is cast to an integer type and than back to a pointer, which under CHERI results in an invalid capability due to missing bounds as they cannot be infered.

```c
typedef struct 
{
#if QLZ_COMPRESSION_LEVEL == 1
 const unsigned char *offset;
#else
 const unsigned char *offset[QLZ_POINTERS];
#endif
} qlz_hash_decompress;
```

A decompression struct is created to contain an offset for compression tracking, which under `QLZ_COMPRESSION_LEVEL 1` is only ever of a pointer type. The snippet below showcases the codeline, which performs the undesirable casting.

```c
offset2 = (const unsigned char *)(size_t)state->hash[hash].offset;
```

The offset is cast and later dereferenced in the code causing a capability fault at runtime. The cast is removed such that the offset can be stored as a dereferencable pointer and a valid capability.

#### Compression investigation

In addition, there is also a 64-bit flag, which enables some additional code segments and provides a macro for casting. The code specifies it enables 64-bit optimization.

```c
#if QLZ_COMPRESSION_LEVEL == 1 && defined QLZ_PTR_64 && QLZ_STREAMING_BUFFER == 0
 #define OFFSET_BASE source
 #define CAST (ui32)(size_t)
#else
 #define OFFSET_BASE 0
 #define CAST
#endif
```

An example of such usage is shown below:

```c
const unsigned char *o;
ui32 hash, cached;

hash = hash_func(fetch);
cached = fetch ^ state->hash[hash].cache;
state->hash[hash].cache = fetch;

o = state->hash[hash].offset + OFFSET_BASE;
state->hash[hash].offset = CAST(src - OFFSET_BASE);
```

This specific code fragment stores an `offset` into the `o` pointer variable. The offset is defined as follows:

```c
typedef struct 
{
#if QLZ_COMPRESSION_LEVEL == 1
 ui32 cache;
#if defined QLZ_PTR_64 && QLZ_STREAMING_BUFFER == 0
 unsigned int offset;
#else
 const unsigned char *offset;
#endif
#else
 const unsigned char *offset[QLZ_POINTERS];
#endif
```

As a result it can be either an integer or pointer type under `QLZ_COMPRESSION_LEVEL 1`. This can cause a similar danger to previous mentioned scenarios under decompression. The evaluation suggests that the capability is always correctly constructed. The `OFFSET_BASE` is created based on the source buffer for compression and when incremented with the offset from the struct a valid pointer is constructed. The result is then stored and and a chunk of the buffer is compressed. The updated value also uses the `OFFSET_BASE` and integer casting to appropriatly traverse it during the compression iteration.

- [QuickLZ fork](https://github.com/ka5p3rr/QuickLZ)

### lzbench

- large compilation project pulling multiple compression libraries into one big repository
- Abandoned for now
- [lzbench](https://github.com/inikep/lzbench), [lzbench fork](https://github.com/ka5p3rr/lzbench)

### minilzo

- Minilzo is a subset of the lzo library intended to be included in other projects
- Can't build it, missing defined variable names
- [minilzo](https://github.com/yuhaoth/minilzo), [lzo](http://www.oberhumer.com/opensource/lzo/), [project include](https://www.gnu.org/software/grub/manual/grub-dev/html_node/minilzo.html)

### miniz

- Having a hard time running the build script, multiple failures caused by the linker
- Abandoned for now
- [miniz](https://github.com/richgel999/miniz)

### liblzf

- Abandoned

## ImageMagick

### Running instructions

First run the configure script, which automatically sets up a building `Makefile` with the default installation location:

```bash
./configure --build=aarch64-unknown-freebsd14.0
```

The `--build=aarch64-unknown-freebsd14.0` flag is used to recognise the custom `aarch64c` achitecture.

Optionally, if you later wish to install the tool set a custom install directory with the `--prefix=PREFIX` flag, e.g. `--prefix=$HOME/im` to install in the user home directory under `im`.

I run my configuration as:

```bash
./configure --build=aarch64-unknown-freebsd14.0 --prefix=$HOME/im
```

Other flags can also be provided to run the building script. The building including feature toggles can be fully customised. To view all the options visit [Install from Source](https://imagemagick.org/script/install-source.php), [Advanced Linux Installation](https://imagemagick.org/script/advanced-linux-installation.php) or run `./configure -h`.

In order to compile the source code on Morello run `gmake` inside the ImageMagick directory.

Now the tool can be run by:

```bash
./utilities/magick
```

The tool can be optionally install the binaries with `gmake install` either to the default `/usr/local` or the `--prefix=` defined directory. Included tests can be run with `gmake check`. To remove all files produced by installation run `gmake uninstall`. In addition, `gmake clean` and `gmake distclean` are also available to remove files produced by compilation using `gmake` and running the `configure` script, respectively.

### Porting

- Several compilation warnings show up during build: `warning: cast from provenance-free integer type to pointer type will give pointer that can not be dereferenced [-Wcheri-capability-misuse]`
- The issues appear in `MagickWand\wand.c` and `MagickCore\distribute-cache.c`.
- Community discussion [here](https://github.com/ImageMagick/ImageMagick/discussions/5380).

The below example is taken from `wand.c` lines 74 to 94 before ImageMagick community changes were made.

```c
WandExport size_t AcquireWandId(void)
{
  static size_t
    id = 0;

  size_t
    wand_id;

  if (wand_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&wand_semaphore);
  LockSemaphoreInfo(wand_semaphore);
  if (wand_ids == (SplayTreeInfo *) NULL)
    wand_ids=NewSplayTree((int (*)(const void *,const void *)) NULL,
      (void *(*)(void *)) NULL,(void *(*)(void *)) NULL);
  wand_id=id++;
  (void) AddValueToSplayTree(wand_ids,(const void *) wand_id,
    (const void *) wand_id);
  instantiate_wand=MagickTrue;
  UnlockSemaphoreInfo(wand_semaphore);
  return(wand_id);
}
```

The compiler warning aries from the fact that an integer type of `wand_id` variable is being cast to a `void *` pointer. As a result a pointer is created from an integer, which is not dereferencable. Capability tag is 0 and bounds cannot be set.

As a result the ImageMagick developers created a solution, which does remove the warnings. A proposed solution uses a `NULL` pointer, which is incremented and passed as the function argument. Such solution has several problems, which are explained in more detail in the ImageMagick [discussion](https://github.com/ImageMagick/ImageMagick/discussions/5380).

The ImageMagick codebase is constrained by keeping the API consistent, such as changes to update function signatures to use type

### Issues

- Toggling features and packages in configuration using `--with-something`, `--without-something` for packages and `--enable-something`, `--disable-something` for features
  - `--disable-docs` disable documentation flag? It seems to build fine with it
  - `--without-x` disable X11? It seems to build fine with it
  - `--disable-installed` is it needed? creates a distributable portable installation

#### Testing results

```
============================================================================
Testsuite summary for ImageMagick 7.1.0-44
============================================================================
# TOTAL: 86
# PASS:  80
# SKIP:  0
# XFAIL: 0
# FAIL:  3
# XPASS: 0
# ERROR: 3
```

Some tests seem to be failing due to not being able to access fonts or other dependencies which are not included in the configuration.

### Notes and links

<!-- - `env LD_LIBRARY_PATH=/path/to/ImageMagick/lib/directory:/path/to/ImageMagick/lib/some-other-directory:/usr/local/lib:/usr/lib:/lib /path/to/ImageMagick/executable` -->
- [ImageMagick](https://imagemagick.org/), [ImageMagick on GitHub](https://github.com/ImageMagick/ImageMagick)

## Miscellaneous porting
