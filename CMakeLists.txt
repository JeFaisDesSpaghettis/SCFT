cmake_minimum_required(VERSION 3.10)
project(SMGP VERSION 0.4.0)

# Define global paths
set(SMGP_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")
set(SMGP_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src" CACHE INTERNAL "")
set(SMGP-SRV_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src/smgp-srv" CACHE INTERNAL "")
set(SMGP-CLT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src/smgp-clt" CACHE INTERNAL "")
string(REPLACE "/" "\\" SMGP_ROOT_DIR_WIN "${LSMGP_ROOT_DIR}\\")

# Macro definitions
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    message("Building Debug Version")
else()
    message("Building Release Version")
    set(RELEASE_BUILD 1 CACHE INTERNAL "")
    add_definitions("-DRELEASE_BUILD=${RELEASE_BUILD}")
endif()

# Flags
set(SMGP_FLAGS "-Wall" "-O3" CACHE INTERNAL "")
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND SMGP_FLAGS "-Wextra")
endif()

if (RELEASE_BUILD EQUAL 1)
    if (WIN32)
        list(APPEND SMGP_FLAGS "-ffile-prefix-map=${SMGP_ROOT_DIR_WIN}=")
    else()
        list(APPEND SMGP_FLAGS "-ffile-prefix-map=${SMGP_ROOT_DIR}=")
    endif()
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND SMGP_FLAGS "-ggdb")
    else()
        list(APPEND SMGP_FLAGS "-g3")
    endif()
endif()

# Build SMGP
include("${SMGP-SRV_SRC_DIR}/CMakeLists.txt")
include("${SMGP-CLT_SRC_DIR}/CMakeLists.txt")

# From https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
# uninstall target
if (NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()
