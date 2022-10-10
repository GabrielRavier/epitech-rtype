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

    void readPacket(Buffer buf)
    {
        this->entityType    = static_cast<EntityType>(buf.readU8());
        this->entityId      = buf.readU16();
        this->posX          = buf.readU16();
        this->posY          = buf.readU16();
        this->life          = buf.readU16();
    };

    void writePacket(Buffer buf)
    {
        buf.writeU8(this->entityType);
        buf.writeU16(this->entityId);
        buf.writeU16(this->posX);
        buf.writeU16(this->posY);
        buf.writeU16(this->life);
    };
};
