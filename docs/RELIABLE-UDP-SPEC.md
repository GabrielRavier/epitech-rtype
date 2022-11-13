Published: November 2022

Author: Gabriel Ravier, Epitech

# RFC - Reliable User Datagram Protocol (RUDP)

## 1. Motivation

Computer communication systems are sometimes required to use the User Datagram Protocol (UDP) [N1] to communicate. This document focuses its attention on robustness in the presence of network unreliability and availability in the presence of congestion, and aims to provide a reliable, connection-oriented, end-to-end protocol alike to the Tranmission Control Protocol (TCP) [I1] that is capable of running on top of the User Datagram Protocol (UDP).

## 2. Introduction

The Reliable User Datagram Protocol (RUDP) is intended for use as a highly reliable host-to-host protocol between hosts in computer communication networks who have to communicate through the User Datagram Protocol (UDP).

This document describes the basic functionality to be performed by the Reliable User Datagram Protocol, the programs that implement it and the interface for programs that require its services.

### 2.1. Requirements Language

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "NOT RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in BCP 14 when, and only when, they appear in all capitals, as shown here.

### 2.2. Key RUDP Concepts

RUDP provides a reliable, in-order, byte-stream service to applications.

The application's byte-stream is conveyed over the network via RUDP segments, with each RUDP segment sent as a User Datagram Protocol (UDP) datagram.

RUDP reliability consists of detecting packet losses (via packet IDs) and errors (via per-segment checksums), along with correction via retransmission.

RUDP is connection oriented, though it does not inherently contain a liveness detection capability.

Data flow is supported bidirectionally over RUDP connections (though there is no requirement for applications to use this capability.

RUDP uses UDP port numbers to identify application services and to multiplex distinct flows between hosts.

## 3. Functional Specification

### 3.1. Header Format

RUDP segments are sent as UDP datagrams. The UDP header carries several information fields, including the source and destination ports. An RUDP header follows the UDP headers, supplying information specific to RUDP.

All integers and other multi-byte fields must be sent in little-endian order, as defined in Internet Experiment Note 137 [N4]

An RUDP header, followed by any user data in the segment, is formatted as follows, where each field follows the previous one in the UDP packet:

```
+-------+----------+------+-------+-----------+-----------------------------------------+-----------+------+
| Magic | Checksum | UUID | Flags | Packet ID | Acknowledgement Next Expected Packet ID | Data Size | Data |
+-------+----------+------+-------+-----------+-----------------------------------------+-----------+------+
```

where:

Magic: 4 bytes

The 4 bytes `0x7F`, `0x52`, `0x55`, `0x50`

Checksum: 8 bytes

The checksum field is a Cyclic Redundancy Check (CRC) [N2] checksum. It must be calculated as specified in ECMA-182, p. 51 [N3], with a polynomial of `0x42F0E1EBA9EA3693` and a width of 64 bits, by applying the CRC algorithm to each byte of the segment header and of the user data.

The RUDP checksum is not optional. The sender **MUST** generate it and the receiver **MUST** check it.

- UUID: 16 bytes

The UUID field is a Universally Unique IDentifier (UUID) as specified in RFC 4122 [N5]. It is a unique identifier for the connection.

- Flags: 1 byte

The Flags field is specified as potentially containing any of 3 flags. The INI flag is set if the least significant bit of the field is set, the ACK flag is set if the second least significant bit of the field is set and the LST flag is set if the third least significant bit of the field is set. Other bits **MUST** be ignored and the sender **SHOULD** not set any of them.

- Packet ID: 8 bytes

The Packet ID of the first data byte in this segment.

- Acknowledgement Next Expected Packet ID: 8 bytes

This field contains the value of the next packet ID the sender of the segment is expecting to receive.

- Data Size: 8 bytes

The size of the data that follows this field.

- Data: Variable length

The user data carried by the RUDP segment.

### 3.2. RUDP Terminology Overview

This section includes an overview of the key terms needed to understand the protocol's operation.

#### 3.2.1. Key Connection State Variables

Before we can discuss the operation of the RUDP implementation in detail, we need to introduce some detailed terminology. The maintenance of an RUDP connection requires maintaining state for several variables. We conceive of these variables being stored in a connection record called a Transmission Control Block (TCB). Among the variables stored in the TCB are the local and remote IP addresses and port numbers, and compartment of the connection, pointers to the user's send and receive buffers, pointers to the retransmit queue and to the current segment. In addition, several variables relating to the send and receive packet IDs are stored in the TCB.

```
+----------+---------------------+
| Variable | Description         |
+----------+---------------------+
| SND.UNA  | Send Unacknowledged |
| SND.NXT  | Send Next           |
+----------+---------------------+
```

#### 3.2.2. State Machine Overview

A connection progresses through a series of states during its lifetime. Briefly the states and their meanings are:

LISTEN: Represents waiting for a connection request from any remote RUDP peer and port.

INI-SENT: Represents waiting for a matching initial packet after having sent an initial packet.

ESTABLISHED: Represents an open connection, data received can be delivered to the user. The normal state of the connection when it is being used to transfer data.

An RUDP connection progresses from one state to another in response to events. The events are the user-usable calls OPEN, SEND, RECEIVE and CLOSE, incoming segments (in particular the INI, ACK and LST flags) and timeouts.

The OPEN call specifies whether establishment of the connection is to be pursued or waited for. A passive OPEN means the user wants to passively wait and accept incoming requests, whereas an active OPEN attempts to initiate a connection.

### 3.3. Packet IDs

A fundamental notion in the design of RUDP is that every segment sent over an RUDP connection has a packet ID. Since every segment is ID-ed, each of them can be acknowledged. This mechanism allows for straightforward duplicate detection in the presence of retransmission. It is essential to remember that the actual packet ID space is finite, though large. This space ranges from 0 to 2^64 - 1. Since the space is finite, all arithmetic dealing with packet IDs must be performed modulo 2^64. This unsigned arithmetic preserves the relationship of packet IDs as they cycle from 2^64 - 1 to 0 again. There are some subtleties to computer modulo arithmetic, so great care should be taken in programming the comparison of such values. The symbol "<=" means "less than or equal" (modulo 2^64).

The typical kinds of packet ID comparisons that the RUDP implementation must perform include:

- Determining that an acknowledgment refers to some packet ID sent but not yet acknowledged.
- Determining that a packet ID occupied by a segment have been acknowledged (e.g., to remove the segment from a retransmission queue).
- Determining that an incoming segment contains a packet ID that is expected

In response to sending data, an RUDP endpoint will receive acknowledgments. The following comparisons are needed to process the acknowledgments:

- `SND.UNA` = Oldest unacknowledged packet ID
- `SND.NXT` = Next packet ID to be sent
- `SEG.ACK` = Acknowledgement from the receiving RUDP peer (next Packet ID expected by them)
- `SEG.PID` = Packet ID of a segment

A new acknowledgement is one for which the following inequality holds:

`SND.UNA < SEG.ACK <= SND.NXT`

A segment on the retransmission queue is fully acknowledged if its Packet ID is less than the acknowledgement value in the incoming segment.

When data is received, the following comparisons are needed:

- `RCV.NXT` = Next Packet ID expected on an incoming segment
- `SEG.PID` = Packet ID occupied by the incoming segment

### 3.4. UUIDs

A connection is defined by a pair of sockets. Connections can be reused. New instances of a connection will be referred to as incarnations of the connection. The problem that arises from this is -- "how does the RUDP implementation identify duplicate segments from previous incarnations of the connection?" This problem becomes apparent if the connection is being opened and closed in quick succession, or if the connection breaks with loss of memory and is then reestablished. To support this, the UUID described below protects against ambiguity about which incarnation of a connection an incoming packet corresponds to.

To avoid confusion, we must prevent segments from one incarnation of a connection from being used while the same packet IDs may still be present in the network from an earlier incarnation. We want to assure this even if a RUDP endpoint loses all knowledge of the packet IDs it has been using. When new connections are created, an random generator is employed that selects a new 128-bit UUID. There are security issues that result if an off-path attacker is able to predict or guess UUID values.

The random generator for the UUID **MUST NOT** be predictable from the outside, or an attacker could plausibly guess at the UUID and hijack a connection.

For a connection to be established, the two RUDP peers **MUST** synchronize on the UUID of the connection.

### 3.5. Establishing a connection

The connection procedure normally is initiated by one RUDP peer and responded to by another RUDP peer. A handshake can occur as such:

- RUDP Peer A has no state and RUDP Peer B is in LISTEN state
- RUDP Peer A sends an empty packet with the INI flag set and a UUID. This puts RUDP Peer A in the INI-SENT state.
- RUDP Peer B receives the packet from RUDP Peer A. This puts RUDP Peer B in the ESTABLISHED state, and it sends back an ACK for the connection.
- RUDP Peer A receives the ACK from RUDP Peer B. This puts RUDP Peer A in the ESTABLISHED state.

Note that simultaneous handshakes are not supported by RUDP.

### 3.6. Closing a connection

CLOSE is an operation meaning there is no more data to send. The RUDP implementation will signal a user, even if no RECEIVEs are outstanding, that the remote peer has closed, so the user can terminate their side gracefully.

There are 3 cases:
- The user initiates by telling the RUDP implementation to CLOSE the connection
- The remote RUDP endpoint initiates by sending a LST control signal
- Both users CLOSE simultaneously

Case 1: Local user initiates the close
In this case, a number of LST segment **SHOULD** be sent (the number is implementation-defined but **SHOULD** be between around 10), after which the connection shall be closed and does not exist anymore.

Case 2: RUDP endpoint receives an LST from the network
If an LST arrives from the network, the receiving RUDP endpoint should tell the user that the connection is closing. The connection is aborted.

Case 3: Both users close simultaneously
A simultaneous CLOSE by users at both ends of a connection causes LST segments to be exchanged. Both will then delete the connection.

### 3.7. Data communication

Once the connection is established, data is communicated by the exchange of segments. Because segments may be lost due to errors (checksum test failure) or network congestion, RUDP uses retransmission to ensure delivery of every segment. Duplicate segments may arrive due to network or RUDP retransmission. As discussed in the section on packet IDs (Section 3.3.), the RUDP implementation performs certain tests on the packet IDs and acknowledgment packet IDs in the segments to verify their acceptability.

The sender of data keeps track of the next packet ID to use in the variable SND.NXT. The receiver of data keeps track of the next packet ID to expect in the variable RCV.NXT. The sender of data keeps track of the oldest unacknowledged packet ID in the variable SND.UNA. If the data flow is momentarily idle and all data sent has been acknowledged, then the three variables will be equal. When the sender creates a segment and transmits it, the sender advances SND.NXT. When the receiver accepts a segment, it advances RCV.NXT and sends an acknowledgment. When the data sender receives an acknowledgment, it advances SND.UNA. The extent to which the values of these variables differ is a measure of the delay in the communication. Note that, once in the ESTABLISHED state, all segments must carry current acknowledgment information.

Once a certain timeout is sent without a packet having been acknowledged, the peer must retransmit it. Note that the timeout is implementation-defined, although it **SHOULD** be low (~10 milliseconds) initially and implement some sort of algorithm to back off if it is observed that too many packages are being sent.

#### 3.7.1. RUDP Connection Failures

An attempt to open an RUDP connection could fail with excessive retransmissions of the INI segment or an ICMP Port Unreachable. INI retransmissions **MUST** be handled in the general way just described for data retransmissions, and connecting must eventually fail if there is no response (note that the delay for this is implementation-defined).

### 3.8. Interfaces

Different RUDP implementations may have different user interfaces, but they **SHOULD** at least have OPEN, SEND, RECEIVE and CLOSE methods (or equivalents), which respectively:
- Open a connection to a given address+port or opens a port in listening mode for connections to be initiated on
- Take data from a user-provided buffer and send it on the indicated connection
- Receive data from the indicated connection and fill a user-provided buffer with it
- Close the connection

## 4. References

### 4.1. Normative References

[N1] Postel, J., "User Datagram Protocol", STD 6, RFC 768, DOI 10.17487/RFC0768, August 1980, <http://www.rfc-editor.org/info/rfc768>.

[N2] W Wesley Peterson & E J Weldon - Error Correcting Codes - First Edition 1961/Second Edition 1972.

[N3] Published by Ecma as Standard ECMA-182 - Data interchange on 12,7 mm 48-track magnetic tape cartridges - DLT1 format - 1st edition, December 1992

[N4] Danny Cohen, "On Holy Wars and a Plea for Peace", USC/ISI, 1 April 1980, <https://www.rfc-editor.org/ien/ien137.txt>

[N5] P. Leach - Microsoft & M.Mealling - Refactored Networks, LLC & R. Salz - DataPower Technology, Inc., July 2005, "A Universally Unique IDentifier (UUID) URN Namespace", <https://www.rfc-editor.org/rfc/rfc4122>

### 4.2. Informative References

[I1] Postel, J., "Transmission Control Protocol", STD 7, RFC 793, DOI 10.17487/RFC0793, September 1981, <https://www.rfc-editor.org/info/rfc793>.
