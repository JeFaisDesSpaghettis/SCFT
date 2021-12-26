#ifndef SCFT_MESSAGE_HPP
#define SCFT_MESSAGE_HPP

/**
 * @file src/scft_message.hpp
 * @brief Defines class message, to send structured information
*/

#include "crc32.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>

/**
 * @brief SCFT General namespace
 * @verbatim
 * [1][2][0003][0004][ORIGIN...][STRINGDATA...]
 * 1: Identifier (MESSAGE_TYPE) 1 byte
 * 2: Origin length 1 byte
 * 3: Stringdata length 4 bytes
 * 4: CRC32 checksum 4 bytes
 * @endverbatim
*/
namespace scft
{
    /**
     * @brief Message related namespace
    */
    namespace message
    {
        /**
         * @brief Message type indetifier
        */
        typedef enum _MESSAGE_TYPE : std::uint8_t
        {
            RESERVED = 0,   //!< No use
            TEXT = 1,       //!< Plain text
            WRITE_FILE = 2  //!< File
        }MESSAGE_TYPE;

        /**
         * @brief Maximum message length 1G
        */
        constexpr std::uint32_t MAX_DATA_LENGTH = 1073741824;

        /**
         * @brief Offset of identifier in a message
        */
        const std::uint32_t MESSAGE_TYPE_OFFSET = 0;

        /**
         * @brief Offset of the origin segment length
        */
        const std::uint32_t ORIGIN_LEN_OFFSET = sizeof(std::uint8_t);

        /**
         * @brief Offset of the stringdata segment length, not to be confused with data
        */
        const std::uint32_t STRINGDATA_LEN_OFFSET = sizeof(std::uint8_t) + sizeof(std::uint8_t);

        /**
         * @brief Offset of the CRC32 checksum
        */
        const std::uint32_t CHECKSUM_OFFSET = sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t);

        /**
         * @brief Offset of data start (origin)
        */
        const std::uint32_t DATA_OFFSET = sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t) + sizeof(std::uint32_t);

        /**
         * @brief Size of the header
        */
        const std::size_t HEADER_SIZE = sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t) + sizeof(std::uint32_t);


        /**
         * @brief Message wrapper
        */
        class message
        {

            /**
             * @brief Initialize internal vector size to HEADER_SIZE
            */
            public: message();

            /**
             * @brief Creates message ready to send
             * @param message_type Throw std::logic_error if it isn't TEXT or WRITE_FILE
             * @param address Sender address
             * @param port Sender port
             * @param _str Text or file name
            */
            public: message(MESSAGE_TYPE message_type, const std::string& address, std::uint16_t port, const std::string& _str);

            /**
             * @brief Intialize message as plain text
             * @param origin Sender string
             * @param text Plain text, accepts u8
            */
            private: void init_as_text(const std::string& origin, const std::string& text);

            /**
             * @brief Initialize message as file
             * @param origin Sender string
             * @param filepath Path to file
            */
            private: void init_as_file(const std::string& origin, const std::string& filepath);

            /**
             * @brief Default destructor
            */
            public: ~message();

            /**
             * @brief Check if header is valid
             * @return True if bad header, false if ok
            */
            public: bool bad_header();

            /**
             * @brief Resizes buffer according to header
            */
            public: void adjust();

            /**
             * @brief Returns message type
             * @return Message type
            */
            public: MESSAGE_TYPE get_message_type();

            /**
             * @brief Return reference to internal buffer
             * @return Access to this class internal vector
            */
            public: std::vector<std::uint8_t>& get_raw_message();

            /**
             * @brief Returns pointer to sender string
             * @return Return pointer to origin
            */
            public: char* get_origin();

            /**
             * @brief Returns pointer to text or file name
             * @return Returns pointer to stringdata start
            */
            public: char* get_string();

            /**
             * @brief Identical to get_origin()
             * @return Returns pointer to data start
            */
            public: char* get_data();

            /**
             * @brief Returns file buffer
             * @return nullptr if there's no file buffer
            */
            public: const std::uint8_t* get_file_buffer();

            /**
             * @brief Returns sender string
             * @return Length of origin
            */
            public: std::uint8_t get_origin_len();

            /**
             * @brief Returns file name or text length
             * @return Stringdata length without file buffer length (it there's any)
            */
            public: std::uint32_t get_string_len();

            /**
             * @brief Returns length of text or (file name length + 1 + file size)
             * @return Stringdata length
            */
            public: std::uint32_t get_stringdata_len();

            /**
             * @brief Same as get_origin_len() + get_stringdata_len()
             * @return Length of the data (origin + stringdata)
            */
            public: std::uint32_t get_data_len();

            /**
             * @brief Returns checksum
             * @return CRC32 Checksum
            */
            public: std::uint32_t get_checksum();

            /**
             * @brief Get file length
             * @return 0, if it does not contain a file
            */
            public: std::uint32_t get_file_buffer_len();

            /**
             * @brief Underlying buffer
            */
            private: std::vector<std::uint8_t> m_raw_message;
        };
    }
}

#endif /* SCFT_MESSAGE_HPP */