cmake_minimum_required(VERSION 3.16)

project(ELEC3885 C CXX ASM)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(lib)

option(ELEC3885_BUILD_TARGET "Build target executable" ON)
mark_as_advanced(ELEC3885_BUILD_TARGET)
if(ELEC3885_BUILD_TARGET)
    include(target)
endif()

option(ELEC3885_BUILD_TESTS "Build unit tests" ON)
mark_as_advanced(ELEC3885_BUILD_TESTS)
if(ELEC3885_BUILD_TESTS)
    include(tests)
endif()

option(ELEC3885_BUILD_DOCS "Build documentation" ON)
mark_as_advanced(ELEC3885_BUILD_DOCS)
if(ELEC3885_BUILD_DOCS)
    include(docs)
endif()
