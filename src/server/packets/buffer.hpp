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
    explicit Buffer(size_t capacity) : m_capacity(capacity), m_data(new uint8_t[capacity]) {}

    ~Buffer() = default;

    uint8_t readU8()
    {
        return this->m_data[this->m_pos++];
    }

    uint16_t readU16()
    {
        auto result = static_cast<uint16_t>(this->m_data[this->m_pos]) << 8 |
                      static_cast<uint16_t>(this->m_data[this->m_pos + 1]);
        this->m_pos += 2;
        return result;
    }

    uint32_t readU32()
    {
        auto result = static_cast<uint32_t>(this->m_data[this->m_pos]) << 24 |
                      static_cast<uint32_t>(this->m_data[this->m_pos + 1]) << 16 |
                      static_cast<uint32_t>(this->m_data[this->m_pos + 2]) << 8 |
                      static_cast<uint32_t>(this->m_data[this->m_pos + 3]);
        this->m_pos += 4;
        return result;
    }

    uint64_t readU64()
    {
        auto result = static_cast<uint64_t>(this->m_data[this->m_pos]) << 56 |
                      static_cast<uint64_t>(this->m_data[this->m_pos + 1]) << 48 |
                      static_cast<uint64_t>(this->m_data[this->m_pos + 2]) << 40 |
                      static_cast<uint64_t>(this->m_data[this->m_pos + 3]) << 32 |
                      static_cast<uint64_t>(this->m_data[this->m_pos + 4]) << 24 |
                      static_cast<uint64_t>(this->m_data[this->m_pos + 5]) << 16 |
                      static_cast<uint64_t>(this->m_data[this->m_pos + 6]) << 8 |
                      static_cast<uint64_t>(this->m_data[this->m_pos + 7]);
        this->m_pos += 8;
        return result;
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
        memmove(str, &this->m_data[this->m_pos], length);
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

    void writeString(const std::string &str)
    {
        this->writeU32(static_cast<uint32_t>(str.length()));
    }

    void appendBuffer(Buffer &buffer, size_t size)
    {
        memmove(&this->m_data[this->m_pos], buffer.data(), size);
        this->m_pos += size;
    }

    void moveToFront(size_t size)
    {
        size -= this->m_pos;
        memmove(&this->m_data[0], &this->m_data[this->m_pos], size);
        this->m_pos = size;
    }

    [[nodiscard]] uint8_t *data()
    {
        return this->m_data.get();
    }

    [[nodiscard]] size_t pos() const
    {
        return this->m_pos;
    }

    [[nodiscard]] size_t capacity() const
    {
        return this->m_capacity;
    }

    void setPos(size_t pos)
    {
        this->m_pos = pos;
    }

private:
    size_t                     m_capacity;
    size_t                     m_pos{0};
    std::unique_ptr<uint8_t[]> m_data;
};
