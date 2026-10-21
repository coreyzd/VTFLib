# VTFLib++ - A Valve VTF and VMT image format programming library.

VTFLib++ is a LGPL open source programming library that provides a C and C++ API that, with a few simple functions, can open and save .vtf and .vmt files, providing access to all known features. The library functions independent of Steam, allowing third party applications to use the library without Steam present or running on the target system.

VTFLib++ includes two GPL example applications, VTFCmd and VTFEdit++. VTFCmd is a C command line frontend for VTFLib++ that can create .vtf and .vmt files from various source formats. It is similar in functionality to Valve's vtex Source SDK utility, but offers a lot more control. VTFEdit++ is a Qt-based graphical frontend for VTFLib++ with viewing and creation capabilities, plus a VMT text editor. Both VTFCmd and VTFEdit++ support several source image formats: .bmp, .dds, .gif, .jpg, .png, .tga, .exr, .pfm, .hdr

## Library/Author Information

* **Title**: VTFLib++
* **Version**: 2.0.0 (VTFEdit++ 4.0.0)
* **Written In**: C/C++
* **Original authors**: [Neil 'Jed' Jedrzejewski](https://github.com/NeilJed) & [Ryan Gregg](http://nemesis.thewavelength.net/), ['misyltoad'](https://github.com/misyltoad)
* **Maintainer**: [ficool2](https://github.com/ficool2)
* **Homepage**: https://ficool2.github.io/HammerPlusPlus-Website

## Project Structure

The repository contains the following folders:

* **lib** - Prebuilt library binaries and the public C header.
* **script** - Helper scripts.
* **thirdparty** - Dependencies, all built from source as git submodules.
* **VTFCmd** - CLI source code.
* **VTFEdit** - Qt program source code
* **VTFLib** - C++ library source code.

## Building with CMake

Requires CMake 3.21+ and a C++17 compiler. Builds for both Windows and Linux.

All third party dependencies are built from source, so the submodules must be checked out first:

```
git submodule update --init --recursive
```

Then build:

```
cmake -B build -DCMAKE_PREFIX_PATH=<path-to-Qt6> -A x64
cmake --build build --config Release
```

### Linux

Install a compiler, CMake, Ninja, NASM (for libjpeg-turbo's SIMD code) and Qt 6 base development files:

```
# Fedora / Nobara
sudo dnf install gcc-c++ cmake ninja-build nasm qt6-qtbase-devel

# Debian / Ubuntu
sudo apt install build-essential cmake ninja-build nasm qt6-base-dev

# Arch
sudo pacman -S base-devel cmake ninja nasm qt6-base
```

Then build (no `-A x64`, and Qt is found automatically when installed from your distro):

```
git submodule update --init --recursive
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/bin/VTFEdit++
```

The binaries in `build/bin` find `libVTFLib++.so` and `libDevIL.so` next to themselves, so the folder can be run in place or copied elsewhere. To install system-wide (adds a desktop entry, icon and `.vtf`/`.vmt` MIME types):

```
sudo cmake --install build            # defaults to /usr/local
# or: cmake --install build --prefix ~/.local
```

### File manager thumbnails

The Linux build also produces thumbnailers, so `.vtf` files show previews instead of a generic icon:

* **`vtf-thumbnailer`** (always built with Qt 6) - for Nautilus (GNOME Files), Nemo, Caja, Thunar, PCManFM and other file managers that follow the freedesktop.org thumbnailer spec. Installed together with `share/thumbnailers/vtf.thumbnailer`.
* **KDE plugin** (built when the KF6 KIO development files are installed) - for Dolphin and other KDE apps.

For the KDE plugin, install its dependencies before configuring:

```
# Fedora / Nobara
sudo dnf install kf6-kio-devel extra-cmake-modules
# Debian / Ubuntu
sudo apt install libkf6kio-dev extra-cmake-modules
# Arch
sudo pacman -S kio extra-cmake-modules
```

After `sudo cmake --install build`:

* **Dolphin:** enable **Valve Texture Format** under *Configure Dolphin → Interface → Previews*, then restart Dolphin. The plugin is installed into Qt's plugin directory (e.g. `/usr/lib64/qt6/plugins/kf6/thumbcreator`), since that is the only place KDE looks; override with `-DVTFLIB_KDE_THUMBNAILER_DIR=...`.
* **Nautilus / Nemo / Caja:** quit the file manager (`nautilus -q`, `nemo -q`) so it rescans thumbnailers.
* **Thunar:** needs `tumbler` installed; run `thunar -q` and restart it.

If a file manager keeps showing the old generic icon, it has cached a failed thumbnail from before installation. Clear it with `rm -rf ~/.cache/thumbnails/fail` (or all of `~/.cache/thumbnails`).

Build options:

* `VTFLIB_BUILD_VTFCMD` (default ON) - build VTFCmd
* `VTFLIB_BUILD_VTFEDIT` (default ON) - build VTFEdit++ (requires Qt 6)
* `VTFLIB_BUILD_THUMBNAILER` (default ON, Linux only) - build `vtf-thumbnailer` for Nautilus, Nemo, Thunar, etc.
* `VTFLIB_BUILD_KDE_THUMBNAILER` (default ON, Linux only) - build the Dolphin thumbnail plugin if KF6 KIO is found

Binaries are written to `build/bin`

## VTFCmd Usage

```
Correct vtfcmd usage:
 -file <path>             (Input file path.)
 -folder <path>           (Input directory search string.)
 -output <path>           (Output directory.)
 -prefix <string>         (Output file prefix.)
 -postfix <string>        (Output file postfix.)
 -version <string>        (Output version.)
 -format <string>         (Output format to use on non-alpha (colour) textures.)
 -compress <integer>      (Compress image data; -1 default, 0 off, 1-9. Requires version 7.6.)
 -cmethod <string>        (Compression method: deflate or zstd.)
 -alphaformat <string>    (Output format to use on alpha textures.)
 -srgb                    (Whether to treat image as sRGB colour space or not)
 -flag <string>           (Output flags to set.)
 -resize                  (Resize the input to a power of 2.)
 -rmethod <string>        (Resize method to use.)
 -rfilter <string>        (Resize filter to use.)
 -rwidth <integer>        (Resize to specific width.)
 -rheight <integer>       (Resize to specific height.)
 -rclampwidth <integer>   (Maximum width to resize to.)
 -rclampheight <integer>  (Maximum height to resize to.)
 -rkeepaspect             (Preserve aspect ratio when clamping; scales both dimensions.)
 -gamma                   (Gamma correct image.)
 -gcorrection <single>    (Gamma correction to use.)
 -distancealpha           (Encode the alpha channel as a distance field.)
 -dspread <single>        (Width of the distance field gradient in output pixels.)
 -dreduce <integer>       (Shrink the image by this factor after computing the field.)
 -dthreshold <integer>    (Source alpha above which a pixel is inside the shape.)
 -nomipmaps               (Don't generate mipmaps.)
 -mfilter <string>        (Mipmap filter to use.)
 -bumpscale <single>      (Engine bump mapping scale to use.)
 -nothumbnail             (Don't generate thumbnail image.)
 -noreflectivity          (Don't calculate reflectivity.)
 -shader <string>         (Create a material for the texture.)
 -param <string> <string> (Add a parameter to the material.)
 -recurse                 (Process directories recursively.)
 -exportformat <string>   (Convert VTF files to the format of this extension.)
 -silent                  (Silent mode.)
 -pause                   (Pause when done.)
 -help                    (Display vtfcmd help.)

Example vtfcmd usage:
vtfcmd.exe -file "C:\texture1.bmp" -file "C:\texture2.bmp" -format "dxt1"
vtfcmd.exe -folder "C:\input\*.tga" -output "C:\output" -recurse -pause
vtfcmd.exe -folder "C:\output\*.vtf" -output "C:\input" -exportformat "jpg"
```

## Program Copyright-Permissions

See the LGPL.txt (VTFLib++) and GPL.txt (VTFCmd & VTFEdit++) files contained in the distribution.
