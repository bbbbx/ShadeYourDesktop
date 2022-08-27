# Shade Your Desktop [![Build macOS](https://github.com/bbbbx/ShadeYourDesktop/actions/workflows/build_macos.yml/badge.svg)](https://github.com/bbbbx/ShadeYourDesktop/actions/workflows/build_macos.yml)

<p align="center">
  <a href="https://vimeo.com/723791479"><img src="./preview.gif" /></a> <br />
</p>
<p align="center">
  Use <strong>GLSL/video</strong> to shade your desktop
</p>

## Dependencies

- [FFmpeg](https://ffmpeg.org/download.html)

### Install FFmpeg with Homebrew

Use [Homebrew](https://brew.sh/) to install them:

```sh
$ brew install ffmpeg
```

### Install FFmpeg from source code

Make sure the git submodules was updated:

```sh
git submodule update --init --recursive
```

Compile shared libraries and install:

```sh
$ cd extern/FFmpeg && \
mkdir build && \
./configure --prefix=$(pwd)/build  --disable-doc --disable-programs --disable-static --enable-shared && \
make -j8 && \
make install
```

The installation directory should be `extern/FFmpeg/build`.

If compilation complains `nasm/yasm not found or too old.`, you can use brew to install `nasm` or `yasm`, and try again.

## Compile

In order to include and link FFmpeg correctly, there are two options you need to specify, or just use the defaults:

- `FFmpeg_INCLUDE`: default is `/opt/homebrew/include`
- `FFmpeg_LIB`: default is `/opt/homebrew/lib`

and make sure FFmpeg shared libraries(.ddl for Windows, .dylib for macOS, .so for Linux) are included your path.

Config CMake from project root directory:

```sh
$ cmake . -B build -DFFmpeg_INCLUDE=<ffmpeg_include_path> -DFFmpeg_LIB=<ffmpeg_lib_path>
```

For example:
1. if you install FFmpeg with Homebrew, `<ffmpeg_include_path>` could be `$(brew --prefix)/include`, and `<ffmpeg_lib_path>` could be `$(brew --prefix)/lib`;
2. if you install FFmpeg from source code, `<ffmpeg_include_path>` could be `$(pwd)/extern/FFmpeg/build/include`, and `<ffmpeg_lib_path>` could be `$(pwd)/extern/FFmpeg/build/lib`.

Then compile:

```sh
$ cmake --build build -j 8 --config Release
```

Executable file will be  placed in `bin` folder.

## Usage

Print help message:

```sh
$ ./bin/ShadeYourDesktop -h
Usage:
  ShadeYourDesktop [options]
Available options:
  -V, --video  Video file name
      --fs     Fragment shader file name
      --t0     texture 0 file name
      --t1     texture 1 file name
      --t2     texture 2 file name
      --t3     texture 3 file name
  -h, --help   Help message
```

Use video as wallpaper:

```sh
$ ./bin/ShadeYourDesktop --video <your_video_path>
```

Use GLSL to shade your desktop:

```sh
$ ./bin/ShadeYourDesktop --fs <GLSL_file> --t0 <your_image_file_for_texture_0>
```
