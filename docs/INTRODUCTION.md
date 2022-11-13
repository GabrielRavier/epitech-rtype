# Developer documentation

This page, along with other pages down this hierarchy, contains information useful to people who would like to work on the code of this project

General documentation on building, running and installing the game is available on the [INSTALL.md](../INSTALL.md) page

The norm for contributions and other guidelines for contributing is available on the [CONTRIBUTING.md](../CONTRIBUTING.md) page

## Specific tutorials

A tutorial on how to edit and create levels is available [here](./LEVEL-EDITING.md)

## Protocols and specifications

This project uses Reliable UDP, which was created for the purposes of having stable TCP-like networking on top of UDP. The specification for it can be found [here](./RELIABLE-UDP-SPEC.md).

The protocol specification for the game's networking use itself (i.e. what's happening on top of the Reliable UDP layer) can be found [here](./R-TYPE-PROTOCOL-SPEC.md).

## Code hierarchy

[src/client/](../src/client/) contains all R-Type client code

[src/client/components](../src/client/components) contains the code for all of the components of the [ECS](./ECS.md) as used in the client

[src/client/core](../src/client/core) contains the core code of the R-Type client, i.e. code necessary to all the other parts, such as much of the [ECS](./ECS.md)'s base code, code that wraps around window handling or code that handles networking

[src/client/systems](../src/client/systems) contains the code for all of the systems of the [ECS](./ECS.md) as used in the client

[src/client/utils](../src/client/utils) contains code for a few miscellaneous things used all throughout the project but aren't some "core" part of it like the [ECS](./ECS.md)

[src/reliable-udp](../src/reliable-udp) contains the code used for reliable networking on top of UDP, as specified in the [Reliable UDP specification](./RELIABLE-UDP-SPEC.md)

[src/server/](../src/server/) contains all R-Type server code

[src/server/components](../src/client/components) contains the code for all of the components of the [ECS](./ECS.md) as used in the server

[src/server/core](../src/client/core) contains the core code of the R-Type server, i.e. code necessary to all the other parts, such as all of the core code around networking used by the rest of the server

[src/server/packets](../src/client/packets) contains the code for all of the packet types used with the R-Type networking, as specified in the [R-Type Protocol specification](./R-TYPE-PROTOCOL-SPEC.md)

[src/server/systems](../src/client/systems) contains the code for all of the systems of the [ECS](./ECS.md) as used in the server

## CI

Documentation on how the CI is set up and what it does is [here](./CI.md)

## The release workflow

Documentation on the workflow associated with creating a release is located [here](./RELEASE-WORKFLOW.md)
