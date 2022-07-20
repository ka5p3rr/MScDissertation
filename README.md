# MSc Dissertation at the University of Edinburgh <!-- omit in toc -->

## Table of contents <!-- omit in toc -->

- [Compression](#compression)
  - [snappy-c](#snappy-c)
  - [zip](#zip)
  - [FastLZ](#fastlz)
  - [zlib](#zlib)
  - [QuickLZ](#quicklz)
  - [lzbench](#lzbench)
  - [minilzo](#minilzo)
  - [miniz](#miniz)
  - [liblzf](#liblzf)
- [ImageMagick](#imagemagick)
  - [Running instructions](#running-instructions)
  - [Issues](#issues)
  - [Notes and links](#notes-and-links)

## Compression

### snappy-c

- Snappy-c is third-party implementation of the snappy algorithm providing C-language bindings
- Compiles on Morello purecap without issues, build using gmake
- Running on Morello provides runtime safety, for example providing wrongly sized buffers results in a runtime error
- Running verify (their testing file) shows no issues, written short bash script to handle running on compression-corpus
- [snappy-c on GitHub](https://github.com/andikleen/snappy-c), [snappy-c fork](https://github.com/ka5p3rr/snappy-c), [snappy-c API](http://halobates.de/snappy.html), [Main project website](https://github.com/google/snappy), [snappy on GitHub](https://github.com/google/snappy)

### zip

- Library had a buffer overflow bug, which was discovered through their provided tests
- Found a fix with Ruize: [issue](https://github.com/kuba--/zip/issues/270)

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

Running the failing test individually results in a `In-address space security exception (core dumped)` error. Caused by a flipped `&&` condition of an array index. More detail provided [here](https://github.com/kuba--/zip/issues/270).

- [zip](https://github.com/kuba--/zip), "This is done by hacking awesome [miniz](https://github.com/richgel999/miniz) library and layering functions on top of the miniz v2.2.0 API."

### FastLZ

- Found a bug, Ruize found a fix, which makes all the tests pass. A buffer overflow caused a `In-address space security exception (core dumped)`.
- Code alteration can be found in the submodule, which is a fork of the original repository. Bug fix included and added `defined(__aarch64__) || defined(_M_ARM64)` to recognise ARM as a 64-bit system.
- [FastLZ](https://github.com/ariya/FastLZ), [FastLZ fork](https://github.com/ka5p3rr/FastLZ)

### zlib

- Compiles with no issues, all tests pass
- [Main project website](http://zlib.net/), [zlib on GitHub](https://github.com/madler/zlib)

### QuickLZ

- Seems to cause a memory fault `In-address space security exception (core dumped)`
- [QuickLZ fork](https://github.com/ka5p3rr/QuickLZ)

### lzbench

- large compilation project pulling multiple compression libraries into one big repository
- Abandoned for now
- [lzbench](https://github.com/inikep/lzbench)

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
$ ./configure --build=aarch64-unknown-freebsd14.0
```

The `--build=aarch64-unknown-freebsd14.0` flag is used to recognise the custom `aarch64c` achitecture.

Optionally, if you later wish to install the tool set a custom install directory with the `--prefix=PREFIX` flag, e.g. `--prefix=$HOME/im` to install in the user home directory under `im`.

Other flags can also be provided to run the building script. The building including feature toggles can be fully customised. To view all the options visit [Install from Source](https://imagemagick.org/script/install-source.php), [Advanced Linux Installation](https://imagemagick.org/script/advanced-linux-installation.php) or run `./configure -h`.

In order to compile the source code on Morello run `gmake` inside the ImageMagick directory.

Now the tool can be run by:

```bash
$ ./utilities/magick
```

The tool can be optionally install the binaries with `gmake install` either to the default `/usr/local` or the `--prefix=` defined directory. Included tests can be run with `gmake check`. To remove all files produced by installation run `gmake uninstall`. In addition, `gmake clean` and `gmake distclean` are also available to remove files produced by compilation using `gmake` and running the `configure` script, respectively.

### Issues

- Several compilation warnings show up during build: `warning: cast from provenance-free integer type to pointer type will give pointer that can not be dereferenced`
- `--disable-docs`? It seems to build fine with it
- `--without-x` disable X11? It seems to build fine with it
- `--disable-installed` is it needed? creates a distributable portable installation

### Notes and links

- `env LD_LIBRARY_PATH=/path/to/ImageMagick/lib/directory:/path/to/ImageMagick/lib/some-other-directory:/usr/local/lib:/usr/lib:/lib /path/to/ImageMagick/executable`
- [ImageMagick](https://imagemagick.org/), [ImageMagick on GitHub](https://github.com/ImageMagick/ImageMagick)
