cmake_minimum_required(VERSION 3.0.0)
project(gzipper VERSION 0.1.0)

# flags
set(COMPILE_TESTS ON)

include(CTest)
enable_testing()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(gzipper ${SRC_DIR}/Gzipper.cpp
                    ${SRC_DIR}/BitStream.cpp
                    ${SRC_DIR}/LookbackOutputStream.cpp)
target_include_directories(gzipper PRIVATE ${INC_DIR})

if(${COMPILE_TESTS})
  # from gtest docs

  configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                  ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                  EXCLUDE_FROM_ALL)

  ## INDIVIDUAL TEST ##
  set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

  # move test files into build folder
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests/testfiles/124.txt.gz ${CMAKE_CURRENT_BINARY_DIR}/124.txt.gz COPYONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests/testfiles/123.txt ${CMAKE_CURRENT_BINARY_DIR}/123.txt COPYONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests/testfiles/compile_commands.json.gz ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json.gz COPYONLY)
  # CRC Hash test
  add_executable(VerifyHeader_test ${TEST_DIR}/VerifyHeader.cpp)
  target_include_directories(VerifyHeader_test PRIVATE ${INC_DIR})
  target_link_libraries(VerifyHeader_test gzipper gtest_main)
  add_test(NAME VerifyHeadersTest COMMAND VerifyHeader_test)

  # BitStream test
  add_executable(BitStream_test ${TEST_DIR}/BitStreamTests.cpp)
  target_include_directories(BitStream_test PRIVATE ${INC_DIR})
  target_link_libraries(BitStream_test gzipper gtest_main)
  add_test(NAME BitStreamTest COMMAND BitStream_test)

  # Huffmantree test
  add_executable(HuffmanTree_test ${TEST_DIR}/HuffmanTreeTests.cpp)
  target_include_directories(HuffmanTree_test PRIVATE ${INC_DIR})
  target_link_libraries(HuffmanTree_test gzipper gtest_main)
  add_test(NAME HuffmanTreeTest COMMAND HuffmanTree_test)

  # outputstream test
  add_executable(LOS_test ${TEST_DIR}/LOSTests.cpp)
  target_include_directories(LOS_test PRIVATE ${INC_DIR})
  target_link_libraries(LOS_test gzipper gtest_main)
  add_test(NAME LOSTest COMMAND LOS_test)

endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
