# Shade Your Desktop

<p align="center">
  <a href="https://vimeo.com/723791479"><img src="./preview.gif" /></a> <br />
</p>
<p align="center">
  Use <strong>GLSL/video</strong> to shade your desktop
</p>

## Dependencies

- [FFmpeg](https://ffmpeg.org/download.html)

Use [Homebrew](https://brew.sh/) to install them:

```sh
$ brew install ffmpeg
```

## Compile

In order to include and link FFmpeg correctly, there are two options you need to specify, or just use the defaults:

- `FFmpeg_INCLUDE`: default is `/opt/homebrew/include`
- `FFmpeg_LIB`: default is `/opt/homebrew/lib`

and make sure FFmpeg shared libraries(.ddl for Windows, .dylib for macOS) are included your path.

Use [CMake](https://cmake.org/) to control compilation process:

```sh
$ cmake . -B build -DFFmpeg_INCLUDE=<ffmpeg_include_path> -DFFmpeg_LIB=<ffmpeg_lib_path>
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
