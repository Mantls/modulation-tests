cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(modulation-tests)

add_library(
    libdsp
    ${CMAKE_CURRENT_SOURCE_DIR}/libdsp/Biquad.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdsp/Biquad.cpp

)
find_library(itpp_lib itpp)



add_executable(
    modulation-test
    bpsk.cpp
    qpsk.cpp
    sim.cpp
)

target_link_libraries(modulation-test PRIVATE "${itpp_lib}" libdsp)