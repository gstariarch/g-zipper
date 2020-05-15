cmake_minimum_required(VERSION 3.0.0)
project(gzipper VERSION 0.1.0)

include(CTest)
enable_testing()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(gzipper ${SRC_DIR}/gzipper.cpp)
target_include_directories(gzipper PUBLIC ${INC_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
