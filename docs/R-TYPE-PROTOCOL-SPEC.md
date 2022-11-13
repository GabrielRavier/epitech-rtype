Published: November 2022

Author: Gabriel Ravier, Epitech

# RFC - R-Type Protocol (RTP)

## 1. Introduction

The R-Type Protocol (RTP) is intended for use for text communications between several clients, all centered around a central server but potentially between different hosts all around the world.

This document describes the functions to be performed by the RTP, the programs that implement it, and the interface to programs that require its services.

### 1.1. Scope

The RTP is intended to provide a reliable protocol for implementing the R-Type game on a network.

### 1.2. Interfaces

The interface between RTP and the lower-level protocol (TCP [I1], RUDP [I2] or other) is essentially unspecified except that it is assumed there is a mechanism whereby data can be send back and forth asynchronously in a reliable way.

### 1.3. Requirements Language

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "NOT RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in BCP 14 when, and only when, they appear in all capitals, as shown here.

## 2. Functional Specification

### 2.1. Client-Server Relation

RTP is an interface between clients and a single central server. Clients send packets to the server, and receive corresponding packets from it. Certain types of packets may only be sent by clients to the server, and other by the server to the client, and some packet types have different meaning depending on whether they are sent to the client or the server.

All integers and other multi-byte fields must be sent in little-endian order, as defined in Internet Experiment Note 137 [N1]

### 2.2. Header format

RTP packets contain a header. An RTP header is situated at the beginning of an RTP packet.

RTP Header Format:

```
+-------------+---------+----------------+-----------------+
| Packet Size | Padding | Packet Type ID | Packet Contents |
+-------------+---------+----------------+-----------------+
```

where:

- Packet Size: 2 bytes

The size of the packet.

- Padding: 4 bytes

**MUST** be ignored.

- Packet Type ID: 1 byte

The ID of the packet's type, as detailed later.

- Packet Contents: Variable length

The packet data carried by the RTP packet. It **MUST** be at least of the size needed by the corresponding packet type to read all parameters, and **SHOULD** be of the exact needed size (although it is allowed to be too large, in which case the rest of the packet is ignored and has no effect).

### 2.3. Packet Parameters

RTP parameters describe diverse types of values. This section describes what how each type is represented when passed through the RTP protocol.

Currently defined parameter types include:

#### 2.3.1. STRING

A STRING describes a sequence of characters, which **MUST** be represented in the UTF-8 encoding [N2]. Each byte of the STRING shall be passed directly without any interpretation, each one after the other. The string shall be prefixed with a 32-bit number, which indicates its length.

#### 2.3.2. TIMESTAMP

A TIMESTAMP is a 64-bit UNIX timestamp, i.e. the amount of seconds since 1970-01-01 00:00:00 +0000 (UTC).

#### 2.3.3. U8

A U8 is an 8-bit unsigned integer.

#### 2.3.3. U16

A U16 is an 16-bit unsigned integer.

#### 2.3.4. U64

A U64 is an 64-bit unsigned integer.

### 2.4. Packet Type IDs

RTP Packet Type IDs are defined as described in the next paragraph. Any unrecognized data type **MUST** result in an error of some kind. Packet Type IDs marked "(client)" **MUST** only be sent from the client to the server, and Packet Type IDs marked "(server)" **MUST** only be sent from the server to the client. The description of each Packet Type ID begins by describing the parameters, and then the action corresponding to that Packet Type ID. Note that the order in which parameters are sent in the packet correspond to the order in which they are listed.

- Login (client) = 0
- Keep Alive (client) = 1
- Input (client) = 2
- Position (client) = 3
- Logout (client) = 4
- Login (server) = 0
- Keep Alive (server) = 1
- Entity Create (server) = 2
- Entity Destroy (server) = 3
- Update Health (server) = 4
- Update Position (server) = 5
- Update Score (server) = 6

#### 2.4.1. Login (client)

```
+--------+
| STRING |
+--------+
```

This request sets the username used by the client.
The passed STRING is the username.

#### 2.4.2. Keep Alive (client)

```
+-----------+
| TIMESTAMP |
+-----------+
```

This request exists to make sure the server knows the client is alive.
The passed TIMESTAMP indicates when the Keep Alive was sent by the sender and can be used to set up a timeout.

#### 2.4.3. Input (client)

```
+----+
| U8 |
+----+
```

This request indicates to the server that the client has entered a certain input.
The U8 shall be interpreted as a press of the Left button if it is 0, a press of the Right button if it is 1, a press of the Up button if it is 2, a press of the Down button if it is 3 and a press of the Shoot button if it is 4.

#### 2.4.4. Position (client)

```
+-----+-----+
| U16 | U16 |
+-----+-----+
```

This request indicates to the server that the client's position has changed.
The first U16 shall be interpreted as the X position of the client, and the second U16 shall be interpreted as the Y position of the client.

#### 2.4.5. Logout (client)

```
+-+
| |
+-+
```

This request indicates to the server that the client is logging out and wishes to end the connection.
It has no parameters.

#### 2.4.6. Login (server)

```
+----+-----+
| U8 | U16 |
+----+-----+
```

This request indicates to the client that a client connected to the server. The U8 is to be taken as a state indicating the status (0 for accept, 1 for deny). The U16 is to be taken as an entity ID to be used in other requests.

#### 2.4.7. Keep Alive (server)

```
+-----------+
| TIMESTAMP |
+-----------+
```

This request exists to make sure the client knows the server is alive.
The passed TIMESTAMP indicates when the Keep Alive was sent by the sender and can be used to set up a timeout.

#### 2.4.8. Entity Create (server)

```
+----+----+----+-----+-----+-----+-----+
| U8 | U8 | U8 | U16 | U16 | U16 | U16 |
+----+----+----+-----+-----+-----+-----+
```

This request indicates to the client that a new entity has been created.
The first U8 determines the entity's type, the second U8 determines the entity's team, the third U8 determines the mob type, the first U16 determines the entity ID, the second U16 determine the X position, the third U16 determines the Y position and the fourth U16 determines the entity's life.

#### 2.4.9. Entity Destroy (server)

```
+-----+
| U16 |
+-----+
```

This request indicates to the client that an entity has been deleted.
The U16 is the entity ID of the entity that was deleted.

#### 2.4.10. Update Health (server)

```
+-----+-----+
| U16 | U16 |
+-----+-----+
```

This request indicates to the client that an entity's life has changed.
The first U16 is the entity ID of the entity hwose life changed. The second U16 is the new value for the entity's life.

#### 2.4.11. Update Position (server)

```
+-----+-----+-----+
| U16 | U16 | U16 |
+-----+-----+-----+
```

This request indicates to the client that an entity's position has changed.
The first U16 is the entity ID of the entity hwose life changed. The second U16 is the new value for the entity's X position. The third U16 is the new value for the entity's Y position.

#### 2.4.12. Update Score (server)

```
+-----+
| U64 |
+-----+
```

This request indicates to the client that the score has changed
The U64 is the new value of the score.

## 3. References

### 3.1. Normative References

[N1] Danny Cohen, "On Holy Wars and a Plea for Peace", USC/ISI, 1 April 1980, <https://www.rfc-editor.org/ien/ien137.txt>

[N2] F. Yergeau, "UTF-8, a transformation format of ISO 10646", RFC 3629, November 2003, <https://www.rfc-editor.org/rfc/rfc3629>

### 3.2. Informative References

[I1] Postel, J., "Transmission Control Protocol", STD 7, RFC 793, DOI 10.17487/RFC0793, September 1981, <https://www.rfc-editor.org/info/rfc793>.

[I2] Gabriel, R., "Reliable User Datagram Protocol", RFC, November 2022, [RELIABLE-UDP-SPEC.md](./RELIABLE-UDP-SPEC.md)
