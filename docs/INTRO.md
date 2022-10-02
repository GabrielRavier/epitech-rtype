# Developer documentation

This page, along with other pages down this hierarchy, contains information useful to people who would like to work on the code of this project.

## Code hierarchy

[src/client/](../src/client/) contains all R-Type client code

[src/client/components](../src/client/components) contains the code for all of the components of the [ECS](./ECS.md)

[src/client/core](../src/client/core) contains the core code of the R-Type client, i.e. code necessary to all the other parts, such as much of the [ECS](./ECS.md)'s base code or code that wraps around window handling, and other things like that

[src/client/systems](../src/client/systems) contains the code for all of the systems of the [ECS](./ECS.md)

[src/client/utils](../src/client/utils) contains code for a few miscellaneous things used all throughout the project but aren't some "core" part of it like the [ECS](./ECS.md)

[src/server/](../src/server/) contains all R-Type server code
