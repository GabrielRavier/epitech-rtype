# R-Type

This project is a multithreaded server and a graphical client for the game R-Type, using an engine of our own design

## Dependencies

Either:
- Install vcpkg
Or:
- Install SFML manually (On Linux, likely through your package manager)

### Installing vcpkg

vcpkg is a submodule of this repository (note: if you did not clone this repository with `--recurse-submodules`, please run `git submodule update --init` before continuing).
You can prepare it for useusing the following command:

```
./vcpkg/bootstrap-vcpkg.sh
```

on Linux/MacOS, and

```
./vcpkg/bootstrap-vcpkg.bat
```

on Windows.

## Building

This project uses CMake, allowing it to be built with a large range of compilers

Generating files for your build system is as simple as executing the following command, from the top directory of this repository:

```
cmake -B build
```

without vcpkg, and

```
cmake -B build -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
```

if you are using vcpkg.

You can then compile the project with this command (again, from the top directory of this repository):

```
cmake --build build
```

Once built, the `r-type_server` and `r-type_client` executables can be found in the `build` folder.
