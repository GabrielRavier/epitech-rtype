#pragma once

#include <stdexcept>
#include <string>
#include <cstring>
#include <memory>
#include <cstdint>
#include <iostream>

class Buffer
{
public:
    Buffer(size_t capacity)
        : m_capacity(capacity), m_pos(0), m_data(new uint8_t[capacity]) {}

    ~Buffer()
    {
        delete this->m_data;
    };

    uint8_t readU8()
    {
        return this->m_data[this->m_pos++];
    }

    uint16_t readU16()
    {
        return static_cast<uint16_t>(this->m_data[this->m_pos++]) << 8
            | static_cast<uint16_t>(this->m_data[this->m_pos++]);
    }

    uint32_t readU32()
    {
        return static_cast<uint32_t>(this->m_data[this->m_pos++]) << 24
            | static_cast<uint32_t>(this->m_data[this->m_pos++]) << 16
            | static_cast<uint32_t>(this->m_data[this->m_pos++]) << 8
            | static_cast<uint32_t>(this->m_data[this->m_pos++]);
    }

    uint64_t readU64()
    {
        return static_cast<uint64_t>(this->m_data[this->m_pos++]) << 56
            | static_cast<uint64_t>(this->m_data[this->m_pos++]) << 48
            | static_cast<uint64_t>(this->m_data[this->m_pos++]) << 40
            | static_cast<uint64_t>(this->m_data[this->m_pos++]) << 32
            | static_cast<uint64_t>(this->m_data[this->m_pos++]) << 24
            | static_cast<uint64_t>(this->m_data[this->m_pos++]) << 16
            | static_cast<uint64_t>(this->m_data[this->m_pos++]) << 8
            | static_cast<uint64_t>(this->m_data[this->m_pos++]);
    }

    char *readString(size_t max)
    {
        size_t length = this->readU32();

        // Check max.
        if (length > max)
            throw std::runtime_error("The string exceeds the maximum value (read).");

        // Check out of bounds.
        if ((this->m_pos + length) > m_capacity)
            throw std::runtime_error("Packet Out Of Bounds Exception (Read).");

        char *str = new char[length + 1];

        // Copy string and set null terminated string.
        memcpy(str, &this->m_data[this->m_pos], length);
        str[length + 1] = 0;

        // Set read position.
        this->m_pos += length;

        return str;
    }

    void writeU8(uint8_t value)
    {
        this->m_data[this->m_pos++] = value;
    }

    void writeU16(uint16_t value)
    {
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 8);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value);
    }

    void writeU32(uint32_t value)
    {
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 24);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 16);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 8);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value);
    }

    void writeU64(uint64_t value)
    {
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 56);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 48);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 40);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 32);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 24);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 16);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value >> 8);
        this->m_data[this->m_pos++] = static_cast<uint8_t>(value);
    }

    void writeString(std::string str)
    {
        this->writeU32(static_cast<uint32_t>(str.length()));
    }

    void appendBuffer(Buffer &buffer, size_t size)
    {
        memcpy(this->m_data + this->m_pos, buffer.data(), size);
        this->m_pos += size;
    }

    void moveToFront(size_t size)
    {
        size -= this->m_pos;
        memcpy(this->m_data, this->m_data + this->m_pos, size);
        this->m_pos = size;
    }

    uint8_t *data()
    {
        return this->m_data;
    }

    size_t pos() const
    {
        return this->m_pos;
    }

    size_t capacity() const
    {
        return this->m_capacity;
    }

    void setPos(size_t pos)
    {
        this->m_pos = pos;
    }

private:
    size_t      m_capacity;
    size_t      m_pos;
    uint8_t     *m_data;
};
