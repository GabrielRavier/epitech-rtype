#pragma once

#include "packet.hpp"

enum EntityType
{
    PLAYER  = 0,
    BULLET  = 1,
    MOB     = 2,
};

class PacketServerEntityCreate : public Packet
{
public:
    EntityType  entityType;
    uint16_t    entityId;
    uint16_t    posX;
    uint16_t    posY;
    uint16_t    life;

public:
    PacketServerEntityCreate(EntityType entityType, uint16_t entityId, uint16_t posX, uint16_t posY, uint16_t life) :
        entityType(entityType),
        entityId(entityId),
        posX(posX),
        posY(posY),
        life(life) {}
    PacketServerEntityCreate()     = default;
    ~PacketServerEntityCreate()    = default;

    void readPacket(Buffer *buffer)
    {
        this->entityType    = static_cast<EntityType>(buffer->readU8());
        this->entityId      = buffer->readU16();
        this->posX          = buffer->readU16();
        this->posY          = buffer->readU16();
        this->life          = buffer->readU16();
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeU8(this->entityType);
        buffer->writeU16(this->entityId);
        buffer->writeU16(this->posX);
        buffer->writeU16(this->posY);
        buffer->writeU16(this->life);
    };

    void processPacket(INetworkHandler *handler);
};
