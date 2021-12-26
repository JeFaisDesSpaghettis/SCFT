cmake_minimum_required(VERSION 3.10)
project(SCFT-SRV VERSION 0.4.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Configure version
configure_file("${SCFT-SRV_SRC_DIR}/scft-srv_version.hpp.in" "${SCFT-SRV_SRC_DIR}/scft-srv_version.hpp")

# Source files
add_executable(SCFT-SRV
    "${SCFT_SRC_DIR}/crc32.cpp"
    "${SCFT_SRC_DIR}/basic_shell.cpp"
    "${SCFT_SRC_DIR}/scft_message.cpp"
    "${SCFT_SRC_DIR}/scrolling_log.cpp"
    "${SCFT-SRV_SRC_DIR}/member.cpp"
    "${SCFT-SRV_SRC_DIR}/room.cpp"
    "${SCFT-SRV_SRC_DIR}/server.cpp"
    "${SCFT-SRV_SRC_DIR}/main.cpp")

# Includes
target_include_directories(SCFT-SRV PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
    "${SCFT_SRC_DIR}"
    "${SCFT-SRV_SRC_DIR}")

# Lower case name
set_target_properties(SCFT-SRV PROPERTIES OUTPUT_NAME scft-srv)

# Get Boost.asio
find_package(Boost COMPONENTS system)
target_include_directories(SCFT-SRV PUBLIC "${Boost_INCLUDE_DIR}")
target_link_libraries(SCFT-SRV PUBLIC "${Boost_LIBRARIES}")
if (WIN32)
    target_link_libraries(SCFT-SRV PUBLIC ws2_32 wsock32)
elseif (UNIX)
    target_link_libraries(SCFT-SRV PUBLIC pthread)
endif()

# Link built libraries
target_link_directories(SCFT-SRV PUBLIC "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_BINARY_DIR}")

# Flags
target_compile_options(SCFT-SRV PUBLIC "${LSCFT_FLAGS}")

# Install
install(TARGETS SCFT-SRV
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
