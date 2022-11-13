# Code organisation

The game's architecture is such that everything is centered around the [ECS](./ECS.md). Effectively everything (or close to it) is stored somewhere in the global `gCoordinator` object, which represents the ECS in its entirety, and effectively all the rest of the code is centered around making use of the ECS's various systems, with a few classes centered around managing this with names such as `NetworkManager` or `SceneManager`.

For example, the client's main loop is contained in a `SceneManager`, which manages the main loop along with the transitions between the different possible scenes (such as the single player mode, the main menu or the multiplayer mode). This is not needed for the server as it serves solely to support the multiplayer mode.

The client and server also make use of `NetworkManager` classes, which serve the game's network needs, in which the client needs to communicate with the server in a pre-determined and stable way.

## The ECS

As the ECS is the most important part of the game, this section dedicates itself to detailing the various uses made of it in the project.

### Components

The ECS has a large amount of components. These are:

#### Enemy

Represents information associated with an enemy type, associating an ID with the required information needed to spawn an enemy. At the beginning of the game, the ECS is filled with entities that each contain one of the enemy types the game can use by default.

#### Level

Represents information associated with a level, in particular a path to a file containing information needed to start a level. At the beginning of the game, the ECS is filled with entities that each contain one of the available levels (as found in `./assets/levels`) that can be played in the game.

#### Movement

Represents the movement of an entity. Any entity that needs to move must have a Movement component filled in with the value of the movement the game wants it to take (along with a Transform component to accordingly modify)

#### Network

Represents client information in an entity. Whenever a client connects, an entity that contains the Network component is filled in with the appropriate information such as to make it possible to communicate with clients over the ECS (note: that entity is also used to contain the player sprite and other associated things, i.e. one can communicate with a client easily if they have the matching entity that also contains the player information/sprite/position/etc.)

#### NetworkEntity

Represents the ID of an entity that is associated with a remote entity on the server. Any entity that needs to be able to be matched to a remote entity must have an accordingly set NetworkEntity with the proper ID.

#### Player

Represents information associated with a player. Any entity that must be considered to be a player needs to have the Player component filled in.

#### Projectile

Represents information associated with a projectile. Any entity that needs to deal damage upon contact needs to have the Projectile component filled in.

#### RigidBody

Represents information associated with a solid entity. Any entity that needs to be solid (so, say, they have a hitbox) needs to have the RigidBody component filled in (along with having a Transform component to get the base of the hitbox from, of course).

#### Sprite

Represents information associated with a displayed entity. Any entity that needs to be displayed on the screen (as they are otherwise invisible) needs to have the Sprite component filled in.

#### Text

Represents information associated with displayed text. Any entity that needs to be displayed as text on the screen needs to have the Text component filled in (along with having a Transform component to have a position to place the text at).

#### Transform

Represents information associated with an entity's position. Any entity that needs to have a position needs to have the Transform component filled in.

#### Weapon

Represents information associated with an entity's weapon. Any entity that needs to have a weapon (in particular if they want to be able to fire projectiles with one) needs to have the Weapon component filled in.

## Network Managers

Both the client and server contain Network Manager classes, although they are slightly different in the two so as to accomodate the different ways in which the client and server operate when using the network.
It should be noted that documentation about the protocol that the server and client use to communicate R-Type information is not located here, but in [this separate page](./R-TYPE-PROTOCOL-SPEC.md), although this page also contains some details about the implementation of it used in the game.
The game also has a separate layer to handle network instability and other problems with using UDP, for which a specification can be found [here](./RELIABLE-UDP-SPEC.md), although this page also contains some details about the implementation of it used in the game.

The network managers are used by the game to handle communication between the server and the client. In particular, a thread is run for the purpose of getting packets from the network (using the `run` method of the Network Manager classes) that the game can later process. They also provide facilities for sending packets (for the server, a `NetworkClientManager` is created for each connection and can be used to send data to a particular client - the `Network` ECS component is used to provide access to clients through the ECS).

Different types of network packets that may be sent are managed through various classes that implement the `Packet` interface, with each new type implementing the `readPacket` (used when creating a packet based on data received from the network), `writePacket` (used when preparing data to be sent on the network) and `processPacket` interfaces (which are called to do packet-specific actions while processing them).

The packet types that implement `Packet` are translated into packets that can be used with the R-Type Protocol using facilities provided in `connection_state.hpp`: in particular, several functions that allow one to obtain a packet's ID based on a pointer to an instance of it, and functions that allow one to construct a packet of a given type based on an ID obtained on the network are provided.

It should be noted that this document does not actually detail the different kinds of packets that the R-Type Protocol is used to communicate, as such information is available in the [R-Type Protocol specification](./R-TYPE-PROTOCOL-SPEC.md) in section 2.4..

## ReliableUdpConnection

Both the client and server's Network Managers make use of an underlying class called `ReliableUdpConnection`, which implements the Reliable User Datagram Protocol as specified in [this document](./RELIABLE-UDP-SPEC.md).

`ReliableUdpConnection` provides several generic interfaces for:

- Opening a connection (both in listening and sending mode)
- Sending packets
- Receiving packets
- Closing connections
- Status checking (`isOpen`, etc.)

which can be used to send arbitrary data on the network, although this project uses it as the underlying low-level protocol for the R-Type Protocol.
