cmake_minimum_required(VERSION 3.10)
project(IRC_Tests)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.10.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Disable -Werror for GoogleTest
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set_target_properties(gtest PROPERTIES COMPILE_FLAGS "-Wno-error")
  set_target_properties(gtest_main PROPERTIES COMPILE_FLAGS "-Wno-error")
endif()

enable_testing()

add_executable(
  test_client
  test_client.cpp
  ../srcs/Client.cpp
)

add_executable(
  test_command_parser
  test_command_parser.cpp
  ../srcs/CommandParser.cpp
)

add_executable(
  test_channel
  test_channel.cpp
  ../srcs/Channel.cpp
  ../srcs/Client.cpp
)

target_link_libraries(
  test_client
  gtest_main
)

target_link_libraries(
  test_command_parser
  gtest_main
)

target_link_libraries(
  test_channel
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_client)
gtest_discover_tests(test_command_parser)
gtest_discover_tests(test_channel)
