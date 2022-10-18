# R-Type

This project is a multithreaded server and a graphical client for the game R-Type, using an engine of our own design.

## Supported compilers

This project is tested and known to be able to be compiled on:
- Visual Studio 2022 on Windows
- GCC 11.2 on Ubuntu 22.04

Note that when using Visual Studio 2022, you should probably use the Visual Studio Developer Prompt to build the project.

## Dependencies

These dependencies can be installed either through vcpkg (see setup instructions later in this README) or through your local package manager:

- SFML
- Loguru
- Boost

### Installing vcpkg

vcpkg is a submodule of this repository.
If you wish to use vcpkg and did not clone this repository with `--recurse-submodules`, please run `git submodule update --init` before continuing.

## Building

This project uses CMake, allowing it to be built with a large range of compilers.

Generating files for your build system is as simple as executing the following command, from the top directory of this repository (i.e. the folder containing this README file):

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

## Running the game

Once built, the `r-type_server` and `r-type_client` executables can be found in the top directory. It should be noted that on certain environments, such as Visual Studio, the output location is unreliable, with the executables often ending up in locations such as `Debug/r-type_client.exe` or `Release/r-type_client.exe`.

In all cases, when running the game, the executables should be ran from the top directory of this repository.

The server should be run as such from the command-line:

```
./r-type_server [port]
```

(an appropriate, available port should be given for the server to use as the `[port]` argument)

and the client as such:

```
./r-type_client [server-host] [port]
```

(`[server-host]` should be replaced by an ip/hostname/domain-name that a server is located at, and `[port]` by the port a server is using at that location)

The paths to the `r-type_server` and `r-type_client` executables should be used in place of `./r-type_server` and `./r-type_client` if they are not located in the top directory of the project.

## Running tests

Should you have followed the previous instructions, running tests should be as simple as using either the following command (everywhere but on Windows):

```
cmake --build build --target test
```

or the following command (only on Windows):

```
cmake --build build --target RUN_TESTS
```

This will run tests set up for this project, and will report the results (success is normally signified by text stating "100% tests passed" somewhere in the output)

## Making a binary package

(preliminary note: this does not aim to make a package containing the source code. To do so, you should probably just use `git archive`)

Should you have followed the previous instructions, creating a binary package for the project should be as simple as using the following command:

```
cmake --build build --target package
```

## Other resources

[This page contains general recommendations to anyone who wants to contribute to this project](./CONTRIBUTING.md)

[Programmer documentation (i.e. information for people who want to dive into the project's code) is here](./docs/INTRODUCTION.md)

[Doxygen-generated documentation](https://gabrielravier.github.io/epitech-rtype/)
