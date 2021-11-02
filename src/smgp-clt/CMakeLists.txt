cmake_minimum_required(VERSION 3.10)
project(SMGP-CLT VERSION 0.4.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Configure version
configure_file("${SMGP-CLT_SRC_DIR}/smgp-clt_version.hpp.in" "${SMGP-CLT_SRC_DIR}/smgp-clt_version.hpp")

# Source files
add_executable(SMGP-CLT
    "${SMGP_SRC_DIR}/crc32.cpp"
    "${SMGP_SRC_DIR}/basic_shell.cpp"
    "${SMGP_SRC_DIR}/smgp_message.cpp"
    "${SMGP_SRC_DIR}/scrolling_log.cpp"
    "${SMGP-CLT_SRC_DIR}/client.cpp"
    "${SMGP-CLT_SRC_DIR}/main.cpp")

# Includes
target_include_directories(SMGP-CLT PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
    "${SMGP_SRC_DIR}"
    "${SMGP-CLT_SRC_DIR}")

# Lower case name
set_target_properties(SMGP-CLT PROPERTIES OUTPUT_NAME smgp-clt)

# Get Boost.asio
find_package(Boost COMPONENTS system)
target_include_directories(SMGP-CLT PUBLIC "${Boost_INCLUDE_DIR}")
target_link_libraries(SMGP-CLT PUBLIC "${Boost_LIBRARIES}")
if (WIN32)
    target_link_libraries(SMGP-CLT PUBLIC ws2_32 wsock32)
elseif (UNIX)
    target_link_libraries(SMGP-CLT PUBLIC pthread)
endif()

# Link built libraries
target_link_directories(SMGP-CLT PUBLIC "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_BINARY_DIR}")

# Flags
target_compile_options(SMGP-CLT PUBLIC "${LSMGP_FLAGS}")

# Install
install(TARGETS SMGP-CLT
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
