cmake_minimum_required(VERSION 3.10)

project(Blah
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)

# GoogleTest requires at least C++11 -- but we'll go for C++17
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# blah

add_executable(blah
	test.c)

target_link_libraries(blah
	PUBLIC
	LRUCachedFile)


# LRUCachedFile

add_library(LRUCachedFile
	STATIC
	LRUCachedFile.c LRUCachedFile.h)

target_include_directories(LRUCachedFile
	PUBLIC
	.)

target_link_libraries(
  LRUCachedFile
  log.c
)

add_executable(
  LRUCachedFile_test
  LRUCachedFile_test.cpp
)
target_link_libraries(
  LRUCachedFile_test
  gtest_main gmock
)

target_link_libraries(
  LRUCachedFile_test
  LRUCachedFile
)

include(GoogleTest)
gtest_discover_tests(LRUCachedFile_test)

# log.c

add_library(log.c
	STATIC
	../log.c/src/log.c ../log.c/src/log.h)

target_include_directories(log.c
	PUBLIC
	../log.c/src)
