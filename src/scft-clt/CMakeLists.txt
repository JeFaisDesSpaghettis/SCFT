cmake_minimum_required(VERSION 3.10)
project(SCFT-CLT VERSION 0.4.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Configure version
configure_file("${SCFT-CLT_SRC_DIR}/scft-clt_version.hpp.in" "${SCFT-CLT_SRC_DIR}/scft-clt_version.hpp")

# Source files
add_executable(SCFT-CLT
    "${SCFT_SRC_DIR}/crc32.cpp"
    "${SCFT_SRC_DIR}/basic_shell.cpp"
    "${SCFT_SRC_DIR}/scft_message.cpp"
    "${SCFT_SRC_DIR}/scrolling_log.cpp"
    "${SCFT-CLT_SRC_DIR}/client.cpp"
    "${SCFT-CLT_SRC_DIR}/main.cpp")

# Includes
target_include_directories(SCFT-CLT PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
    "${SCFT_SRC_DIR}"
    "${SCFT-CLT_SRC_DIR}")

# Lower case name
set_target_properties(SCFT-CLT PROPERTIES OUTPUT_NAME scft-clt)

# Get Boost.asio
if (CMAKE_SYSTEM_NAME MATCHES "Android")
    find_package(Boost COMPONENTS system filesystem)
else()
    find_package(Boost COMPONENTS system)
endif()

target_include_directories(SCFT-CLT PUBLIC "${Boost_INCLUDE_DIR}")
target_link_libraries(SCFT-CLT PUBLIC "${Boost_LIBRARIES}")
if (WIN32)
    target_link_libraries(SCFT-CLT PUBLIC ws2_32 wsock32)
elseif (UNIX)
    target_link_libraries(SCFT-CLT PUBLIC pthread)
endif()

# Link built libraries
target_link_directories(SCFT-CLT PUBLIC "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_BINARY_DIR}")

# Flags
target_compile_options(SCFT-CLT PUBLIC "${LSCFT_FLAGS}")

# Install
install(TARGETS SCFT-CLT
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
