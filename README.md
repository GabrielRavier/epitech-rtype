# R-Type

This project is a multithreaded server and a graphical client for the game R-Type, using an engine of our own design

## Dependencies

TODO !!!!!

Other than this, the dependencies will be automatically installed as part of the build process.

## Building

This project uses CMake, allowing it to be built with a large range of compilers

Generating files for your build system is as simple as executing the following command, from the top directory of this repository:

```
cmake -B build
```

You can then compile the project with this command (again, from the top directory of this repository):

```
cmake --build build
```

Once built, the `r-type_server` and `r-type_client` executables can be found in the `build` folder.
