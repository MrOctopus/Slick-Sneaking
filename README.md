# Slick Sneaking
[![](https://github.com/MrOctopus/slick-sneaking/actions/workflows/build.yml/badge.svg)](https://github.com/MrOctopus/slick-sneaking/actions/workflows/build.yml)

Sneaking the way Dovahkiin intended it.

## Features
* Slit and choke throats by using the main attack button
* Ensure no-one goes to sleep because they are afraid you'll stab them

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++

### Optional
* [The Elder Scrolls V: Skyrim Special Edition](https://store.steampowered.com/app/489830)
	* Add the environment variable `Skyrim64Path` to point to the root installation of your game directory (the one containing `SkyrimSE.exe`).
* [Mod Organizer 2](https://www.nexusmods.com/skyrimspecialedition/mods/6194)
	* Add the environment variable `SkyrimMoPath` to point to the root installation of your Mo2 SkyrimSE directory (the one containing `ModOrganizer.exe`).

## Building
```
git submodule init
git submodule update
cmake --preset vs2022-windows
cmake --build build --config Release
```

## Tips
* Set `COPY_OUTPUT` or `COPY_OUTPUT_MO` to `ON` to automatically copy the built dll to the game/mo directory
	* `cmake --preset vs2022-windows -DCOPY_OUTPUT=ON`
	* `cmake --preset vs2022-windows -DCOPY_OUTPUT_MO=ON`
* Build the `package` target to automatically build and zip up your dll in a ready-to-distribute format.
	* `cmake --build build --config Release --target package`

## Licenses

All files in this repository are released under the [MIT License](LICENSE.md)
