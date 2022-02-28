
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

# Endian

add_library(Endian
	STATIC
	Endian.c Endian.h)

target_include_directories(Endian
	PUBLIC
	.)

add_executable(
  Endian_test
  Endian_test.cpp
)
target_link_libraries(
  Endian_test
  gtest_main gmock
)

target_link_libraries(
  Endian_test
  Endian
)

include(GoogleTest)
gtest_discover_tests(Endian_test)

# LRUCachedFileReader

add_library(LRUCachedFileReader
	STATIC
	LRUCachedFileReader.c LRUCachedFileReader.h)

target_include_directories(LRUCachedFileReader
	PUBLIC
	.)

target_link_libraries(
  LRUCachedFileReader
  LRUCachedFile Endian
)

add_executable(
  LRUCachedFileReader_test
  LRUCachedFileReader_test.cpp
)
target_link_libraries(
  LRUCachedFileReader_test
  gtest_main gmock
)

target_link_libraries(
  LRUCachedFileReader_test
  LRUCachedFileReader
)

include(GoogleTest)
gtest_discover_tests(LRUCachedFileReader_test)

# HunkFileParser

add_library(HunkFileParser
	STATIC
	HunkFileParser.c HunkFileParser.h)

target_include_directories(HunkFileParser
	PUBLIC
	.)

target_link_libraries(
  HunkFileParser
  LinearAllocator LRUCachedFile LRUCachedFileReader log.c
)

add_executable(
  HunkFileParser_test
  HunkFileParser_test.cpp
)
target_link_libraries(
  HunkFileParser_test
  gtest_main gmock
)

target_link_libraries(
  HunkFileParser_test
  HunkFileParser
)

include(GoogleTest)
gtest_discover_tests(HunkFileParser_test)

# LinearAllocator

add_library(LinearAllocator
	STATIC
	LinearAllocator.c LinearAllocator.h)

target_include_directories(LinearAllocator
	PUBLIC
	.)

target_link_libraries(
  LinearAllocator
  log.c
)

add_executable(
  LinearAllocator_test
  LinearAllocator_test.cpp
)
target_link_libraries(
  LinearAllocator_test
  gtest_main gmock
)

target_link_libraries(
  LinearAllocator_test
  LinearAllocator
)

include(GoogleTest)
gtest_discover_tests(LinearAllocator_test)

# log.c

add_library(log.c
	STATIC
	../log.c/src/log.c ../log.c/src/log.h)

target_include_directories(log.c
	PUBLIC
	../log.c/src)
