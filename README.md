# MSc Dissertation at the University of Edinburgh <!-- omit in toc -->

## Table of contents <!-- omit in toc -->

- [Compression](#compression)
  - [snappy-c](#snappy-c)
  - [FastLZ](#fastlz)
  - [QuickLZ](#quicklz)
  - [liblzf](#liblzf)
  - [lzbench](#lzbench)
  - [minilzo](#minilzo)
  - [miniz](#miniz)
  - [zip](#zip)
  - [zlib](#zlib)
- [ImageMagick](#imagemagick)

## Compression

### snappy-c

- Snappy-c is third-party implementation of the snappy algorithm providing C-language bindings
- Compiles on Morello purecap without issues, build using gmake
- Running on Morello provides runtime safety, for example providing wrongly sized buffers results in a runtime error
- Running verify (their testing file) shows no issues, written short bash script to handle running on compression-corpus
- [snappy-c on GitHub](https://github.com/andikleen/snappy-c), [snappy-c fork](https://github.com/ka5p3rr/snappy-c), [snappy-c API](http://halobates.de/snappy.html), [Main project website](https://github.com/google/snappy), [snappy on GitHub](https://github.com/google/snappy)

### FastLZ

- Found a bug, Ruize found a fix, which makes all the tests pass
- Fork contains a fix to support Arm 64-bit architecture and fix a bug
- [FastLZ](https://github.com/ariya/FastLZ), [FastLZ fork](https://github.com/ka5p3rr/FastLZ)

### QuickLZ

- Seems to cause a memory fault
- [QuickLZ fork](https://github.com/ka5p3rr/QuickLZ)

### liblzf

### lzbench

- [lzbench](https://github.com/inikep/lzbench)

### minilzo

- Minilzo is a subset of the lzo library intended to be included in other projects
- Can't build it, missing defined variable names
- [minilzo](https://github.com/yuhaoth/minilzo), [lzo](http://www.oberhumer.com/opensource/lzo/), [project include](https://www.gnu.org/software/grub/manual/grub-dev/html_node/minilzo.html)

### miniz

- Having a hard time running the build script
- [miniz](https://github.com/richgel999/miniz)

### zip

- Library had a buffer overflow bug, which was discovered through their provided tests
- Found a fix with Ruize: [issue](https://github.com/kuba--/zip/issues/270)
- "This is done by hacking awesome [miniz](https://github.com/richgel999/miniz) library and layering functions on top of the miniz v2.2.0 API."
- [zip](https://github.com/kuba--/zip)

### zlib

- Compiles with no issues, all tests pass
- [Main project website](http://zlib.net/), [zlib on GitHub](https://github.com/madler/zlib)

## ImageMagick

To configure running for the unrecognised CheriBSD, which is based on FreeBSD:

```bash
./configure --build=aarch64-unknown-freebsd14.0
```

Other flags can also be provided to run the building script.

- `env LD_LIBRARY_PATH=/path/to/ImageMagick/lib/directory:/path/to/ImageMagick/lib/some-other-directory:/usr/local/lib:/usr/lib:/lib /path/to/ImageMagick/executable`
- [ImageMagick](https://imagemagick.org/), [ImageMagick on GitHub](https://github.com/ImageMagick/ImageMagick)
