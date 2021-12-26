#include "crc32.hpp"

namespace scft
{
    namespace crc32
    {
        std::uint32_t get_crc32(const void* buffer, std::size_t size, std::uint32_t crc32)
        {
            const std::uint8_t* cur_byte = static_cast<const std::uint8_t*>(buffer);
            while (size-- != 0)
                crc32 = ((crc32 >> 8) & 0x00FFFFFFL) ^ (crc32_table[(crc32 ^ *cur_byte++) & 0xFF]);
            return crc32;
        }

        std::uint32_t get_crc32(std::ifstream& in_file, std::uint32_t crc32)
        {
            std::vector<std::uint8_t> buffer;
            buffer.resize(BUFFER_SIZE);
            while(!in_file.eof())
            {
                in_file.read(reinterpret_cast<char*>(buffer.data()), BUFFER_SIZE);
                crc32 = get_crc32(buffer.data(), in_file.gcount(), crc32);
            }
            return (crc32 ^ (~0));
        }

        std::uint32_t get_crc32(const std::string& in_filename, std::uint32_t crc32)
        {
            std::ifstream in_file = std::ifstream(in_filename, std::ios::in | std::ios::binary);
            get_crc32(in_file, crc32);
            in_file.close();
            return (crc32 ^ (~0));
        }
    }
}