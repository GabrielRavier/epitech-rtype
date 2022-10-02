# R-Type

This project is a multithreaded server and a graphical client for the game R-Type, using an engine of our own design.

## Dependencies

These dependencies can be installed either through vcpkg (including automatically, through using git submodules) or through your local package manager:

- SFML
- Loguru

### Installing vcpkg

vcpkg is a submodule of this repository.
If you wish to use vcpkg and did not clone this repository with `--recurse-submodules`, please run `git submodule update --init` before continuing.

## Building

This project uses CMake, allowing it to be built with a large range of compilers.

Generating files for your build system is as simple as executing the following command, from the top directory of this repository:

```
cmake -B build
```

without vcpkg, and

```
cmake -B build "-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake"
```

if you are using vcpkg (this will automatically install all necessary dependencies).

You can then compile the project with this command (again, from the top directory of this repository):

```
cmake --build build
```

Once built, the `r-type_server` and `r-type_client` executables can be found in the `build` folder.

## Other resources

[This page contains general recommendations to anyone who wants to contribute to this project](./CONTRIBUTING.md)

[Programmer documentation (i.e. information for people who want to dive into the project's code) is here](./docs/INTRO.md)
