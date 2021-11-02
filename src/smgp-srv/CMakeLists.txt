cmake_minimum_required(VERSION 3.10)
project(SMGP-SRV VERSION 0.4.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Configure version
configure_file("${SMGP-SRV_SRC_DIR}/smgp-srv_version.hpp.in" "${SMGP-SRV_SRC_DIR}/smgp-srv_version.hpp")

# Source files
add_executable(SMGP-SRV
    "${SMGP_SRC_DIR}/crc32.cpp"
    "${SMGP_SRC_DIR}/basic_shell.cpp"
    "${SMGP_SRC_DIR}/smgp_message.cpp"
    "${SMGP_SRC_DIR}/scrolling_log.cpp"
    "${SMGP-SRV_SRC_DIR}/member.cpp"
    "${SMGP-SRV_SRC_DIR}/room.cpp"
    "${SMGP-SRV_SRC_DIR}/server.cpp"
    "${SMGP-SRV_SRC_DIR}/main.cpp")

# Includes
target_include_directories(SMGP-SRV PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
    "${SMGP_SRC_DIR}"
    "${SMGP-SRV_SRC_DIR}")

# Lower case name
set_target_properties(SMGP-SRV PROPERTIES OUTPUT_NAME smgp-srv)

# Get Boost.asio
find_package(Boost COMPONENTS system)
target_include_directories(SMGP-SRV PUBLIC "${Boost_INCLUDE_DIR}")
target_link_libraries(SMGP-SRV PUBLIC "${Boost_LIBRARIES}")
if (WIN32)
    target_link_libraries(SMGP-SRV PUBLIC ws2_32 wsock32)
elseif (UNIX)
    target_link_libraries(SMGP-SRV PUBLIC pthread)
endif()

# Link built libraries
target_link_directories(SMGP-SRV PUBLIC "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_BINARY_DIR}")

# Flags
target_compile_options(SMGP-SRV PUBLIC "${LSMGP_FLAGS}")

# Install
install(TARGETS SMGP-SRV
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
