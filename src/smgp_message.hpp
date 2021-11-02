#ifndef SMGP_MESSAGE_HPP
#define SMGP_MESSAGE_HPP

#include "crc32.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace smgp
{
    namespace message
    {
        typedef enum _MESSAGE_TYPE : std::uint8_t
        {
            RESERVED = 0,
            TEXT = 1,
            WRITE_FILE = 2
        }MESSAGE_TYPE;

        constexpr std::uint32_t MAX_DATA_LENGTH = 1073741824;
        const std::uint32_t MESSAGE_TYPE_OFFSET = 0;
        const std::uint32_t ORIGIN_LEN_OFFSET = sizeof(std::uint8_t);
        const std::uint32_t STRINGDATA_LEN_OFFSET = sizeof(std::uint8_t) + sizeof(std::uint8_t);
        const std::uint32_t CHECKSUM_OFFSET = sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t);
        const std::uint32_t DATA_OFFSET = sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t) + sizeof(std::uint32_t);
        const std::size_t HEADER_SIZE = sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t) + sizeof(std::uint32_t);

        class message
        {
            public: message();

            public: message(const std::string& origin, const std::string& _str);

            public: message(MESSAGE_TYPE message_type, const std::string& address, std::uint16_t port, const std::string& _str);

            private: void init_as_text(const std::string& origin, const std::string& text);

            private: void init_as_file(const std::string& origin, const std::string& filepath);

            public: ~message();

            public: bool bad_header();

            public: void adjust();

            public: MESSAGE_TYPE get_message_type();

            public: std::vector<std::uint8_t>& get_raw_message();

            public: char* get_origin();

            public: char* get_string();

            public: char* get_data();

            public: const std::uint8_t* get_file_buffer();

            public: std::uint8_t get_origin_len();

            public: std::uint32_t get_string_len();

            public: std::uint32_t get_stringdata_len();

            public: std::uint32_t get_data_len();

            public: std::uint32_t get_checksum();

            public: std::uint32_t get_file_buffer_len();

            private: std::vector<std::uint8_t> m_raw_message;
        };
    }
}

#endif /* SMGP_MESSAGE_HPP */