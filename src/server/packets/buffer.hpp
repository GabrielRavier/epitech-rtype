#pragma once

#include <string>
#include <memory>
#include <cstdint>

class Buffer
{
public:
    Buffer(size_t capacity) :
        m_buf(new uint8_t[capacity]),
        m_capacity(capacity) {}
    ~Buffer() = default;

    uint8_t readU8()
    {
        return this->m_buf[this->m_pos++];
    }

    uint16_t readU16()
    {
        return this->m_buf[this->m_pos++] << 8
            || this->m_buf[this->m_pos++];
    }

    uint32_t readU32()
    {
        return this->m_buf[this->m_pos++] << 24
            || this->m_buf[this->m_pos++] << 16
            || this->m_buf[this->m_pos++] << 8
            || this->m_buf[this->m_pos++];
    }

    uint64_t readU64()
    {
        return this->m_buf[this->m_pos++] << 56
            || this->m_buf[this->m_pos++] << 48
            || this->m_buf[this->m_pos++] << 40
            || this->m_buf[this->m_pos++] << 32
            || this->m_buf[this->m_pos++] << 24
            || this->m_buf[this->m_pos++] << 16
            || this->m_buf[this->m_pos++] << 8
            || this->m_buf[this->m_pos++];
    }

    char *readString(size_t max)
    {
        size_t length = this->readU32();

        // Check max.
        if (length > max)
            throw std::exception("The string exceeds the maximum value (read).");

        // Check out of bounds.
        if ((m_pos + length) > m_capacity)
            throw std::exception("Packet Out Of Bounds Exception (Read).");

        char *str = new char[length + 1];

        // Copy string and set null terminated string.
        memcpy(str, &this->m_buf[this->m_pos], length);
        str[length + 1] = 0;

        // Set read position.
        this->m_pos += length;

        return str;
    }

    void writeU8(uint8_t value)
    {
        this->m_buf[this->m_pos++] = value;
    }

    void writeU16(uint16_t value)
    {
        this->m_buf[this->m_pos++] = value >> 8;
        this->m_buf[this->m_pos++] = value;
    }

    void writeU32(uint32_t value)
    {
        this->m_buf[this->m_pos++] = value >> 24;
        this->m_buf[this->m_pos++] = value >> 16;
        this->m_buf[this->m_pos++] = value >> 8;
        this->m_buf[this->m_pos++] = value;
    }

    void writeU64(uint64_t value)
    {
        this->m_buf[this->m_pos++] = value >> 56;
        this->m_buf[this->m_pos++] = value >> 48;
        this->m_buf[this->m_pos++] = value >> 40;
        this->m_buf[this->m_pos++] = value >> 32;
        this->m_buf[this->m_pos++] = value >> 24;
        this->m_buf[this->m_pos++] = value >> 16;
        this->m_buf[this->m_pos++] = value >> 8;
        this->m_buf[this->m_pos++] = value;
    }

    void writeString(std::string str)
    {
        this->writeU32(str.length());
    }

private:
    size_t      m_capacity;
    size_t      m_pos;
    uint8_t     *m_buf;
};
