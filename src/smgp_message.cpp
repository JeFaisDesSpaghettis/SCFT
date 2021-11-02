#include "smgp_message.hpp"

namespace smgp
{
    namespace message
    {
        message::message()
        {
            m_raw_message.resize(HEADER_SIZE);
        }

        message::message(MESSAGE_TYPE message_type, const std::string& address, std::uint16_t port, const std::string& _str)
        {
            if (message_type == TEXT)
            {
                std::string origin = address + ":" + std::to_string(port);
                init_as_text(origin, _str);
            }
            else if (message_type == WRITE_FILE)
            {
                std::string origin = address + ":" + std::to_string(port);
                init_as_file(origin, _str);
            }
            else
                throw std::runtime_error("Unrecognized message type\n");
        }

        void message::init_as_text(const std::string& origin, const std::string& text)
        {
            m_raw_message.resize(HEADER_SIZE + origin.size() + 1 + text.size() + 1);

            *reinterpret_cast<std::uint8_t*>(m_raw_message.data() + MESSAGE_TYPE_OFFSET) = static_cast<std::uint8_t>(MESSAGE_TYPE::TEXT);
            *reinterpret_cast<std::uint8_t*>(m_raw_message.data() + ORIGIN_LEN_OFFSET) = static_cast<std::uint8_t>(origin.size() + 1);
            *reinterpret_cast<std::uint32_t*>(m_raw_message.data() + STRINGDATA_LEN_OFFSET) = static_cast<std::uint32_t>(text.size() + 1);
            std::memcpy(m_raw_message.data() + DATA_OFFSET, origin.data(), origin.size() + 1);
            std::memcpy(m_raw_message.data() + DATA_OFFSET + origin.size() + 1, text.data(), text.size() + 1);
            *reinterpret_cast<std::uint32_t*>(m_raw_message.data() + CHECKSUM_OFFSET) =
                crc32::get_crc32(reinterpret_cast<const std::uint8_t*>(m_raw_message.data() + DATA_OFFSET), origin.size() + 1 + text.size() + 1);
        }

        void message::init_as_file(const std::string& origin, const std::string& filepath)
        {
            std::ifstream in_file{filepath, std::ios::in | std::ios::binary | std::ios::ate};
            std::size_t file_size = in_file.tellg();
            in_file.seekg(0, std::ios::beg);
            in_file.clear();

            std::string out_filepath = filepath.substr(filepath.find_last_of("/\\") + 1);

            m_raw_message.resize(HEADER_SIZE + origin.size() + 1 + out_filepath.size() + 1 + file_size);

            *reinterpret_cast<std::uint8_t*>(m_raw_message.data() + MESSAGE_TYPE_OFFSET) = static_cast<std::uint8_t>(MESSAGE_TYPE::WRITE_FILE);
            *reinterpret_cast<std::uint8_t*>(m_raw_message.data() + ORIGIN_LEN_OFFSET) = static_cast<std::uint8_t>(origin.size() + 1);
            *reinterpret_cast<std::uint32_t*>(m_raw_message.data() + STRINGDATA_LEN_OFFSET) = static_cast<std::uint32_t>(out_filepath.size() + 1 + file_size);
            std::memcpy(m_raw_message.data() + DATA_OFFSET, origin.data(), origin.size() + 1);
            std::memcpy(m_raw_message.data() + DATA_OFFSET + origin.size() + 1, out_filepath.data(), out_filepath.size() + 1);
            in_file.read(reinterpret_cast<char*>(m_raw_message.data() + DATA_OFFSET + origin.size() + 1 + out_filepath.size() + 1), file_size);
            in_file.close();
            *reinterpret_cast<std::uint32_t*>(m_raw_message.data() + CHECKSUM_OFFSET) =
                crc32::get_crc32(reinterpret_cast<const std::uint8_t*>(m_raw_message.data() + DATA_OFFSET), origin.size() + 1 + out_filepath.size() + 1 + file_size);
        }

        message::~message()
        {
        }

        bool message::bad_header()
        {
            if (get_message_type() != TEXT && get_message_type() != WRITE_FILE)
                return true;
            if (get_stringdata_len() > MAX_DATA_LENGTH)
                return true;
            return false;
        }

        void message::adjust()
        {
            m_raw_message.resize(HEADER_SIZE + get_data_len());
        }

        MESSAGE_TYPE message::get_message_type()
        {
            MESSAGE_TYPE message_type = static_cast<MESSAGE_TYPE>(*reinterpret_cast<std::uint8_t*>(m_raw_message.data() + MESSAGE_TYPE_OFFSET));
            return message_type;
        }

        std::vector<std::uint8_t>& message::get_raw_message()
        {
            return m_raw_message;
        }

        char* message::get_origin()
        {
            return reinterpret_cast<char*>(m_raw_message.data() + DATA_OFFSET);
        }

        char* message::get_string()
        {
            return reinterpret_cast<char*>(m_raw_message.data() + DATA_OFFSET + get_origin_len());
        }

        char* message::get_data()
        {
            return reinterpret_cast<char*>(m_raw_message.data() + DATA_OFFSET);
        }

        const std::uint8_t* message::get_file_buffer()
        {
            if (get_message_type() != WRITE_FILE)
                return nullptr;
            return m_raw_message.data() + DATA_OFFSET + get_origin_len() + std::strlen(get_string()) + 1;
        }

        std::uint8_t message::get_origin_len()
        {
            std::uint8_t origin_len = *reinterpret_cast<std::uint8_t*>(m_raw_message.data() + ORIGIN_LEN_OFFSET);
            return origin_len;
        }

        std::uint32_t message::get_string_len()
        {
            return std::strlen(get_string());
        }

        std::uint32_t message::get_stringdata_len()
        {
            std::uint32_t stringdata_len = *reinterpret_cast<std::uint32_t*>(m_raw_message.data() + STRINGDATA_LEN_OFFSET);
            return stringdata_len;
        }

        std::uint32_t message::get_data_len()
        {
            return get_origin_len() + get_stringdata_len();
        }

        std::uint32_t message::get_checksum()
        {
            std::uint32_t checksum = *reinterpret_cast<std::uint32_t*>(m_raw_message.data() + CHECKSUM_OFFSET);
            return checksum;
        }

        std::uint32_t message::get_file_buffer_len()
        {
            if (get_message_type() != WRITE_FILE)
                return 0;
            return get_stringdata_len() - std::strlen(get_string()) - 1;
        }
    }
}