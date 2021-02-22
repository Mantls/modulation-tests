cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(modulation-tests)

add_library(
    libdsp
    ${CMAKE_CURRENT_SOURCE_DIR}/libdsp/Biquad.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdsp/Biquad.cpp

)
if (UNIX)
    message(STATUS "UNIX OS: Using PkgConfig")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ITPP_LIB itpp)
    include_directories(${ITPP_LIB_INCLUDE_DIRS})
    link_directories(${ITPP_LIB_LIBRARY_DIRS})
    add_definitions(${ITPP_LIB_CFLAGS_OTHER})
endif()

add_executable(
    modulation-test
    bpsk.cpp
    qpsk.cpp
    qam16.cpp
    sim.cpp
)

target_link_libraries(modulation-test PRIVATE "${ITPP_LIB}" libdsp)