
set(C_SOURCES
  Endian.c Endian.h
  HunkFileParser.c HunkFileParser.h
  LinearAllocator.c LinearAllocator.h
  LRUCachedFile.c LRUCachedFile.h
  LRUCachedFileReader.c LRUCachedFileReader.h
  TestDescriptor.h
  )

set(CXX_TESTS
  Endian_test.cpp
  HunkFileParser_test.cpp
  LinearAllocator_test.cpp
  LRUCachedFile_test.cpp
  LRUCachedFileReader_test.cpp
  )

# blah

add_library(blah
	STATIC
	${C_SOURCES})

target_include_directories(blah
	PUBLIC
	.)

target_link_libraries(
  blah
  log.c
)

if(INCLUDE_TESTS)
  add_executable(
    blah_test
    ${CXX_TESTS}
  )
  target_link_libraries(
    blah_test
    gtest_main gmock
  )

  target_link_libraries(
    blah_test
    blah
  )

  include(GoogleTest)
  gtest_discover_tests(blah_test)
endif()

# log.c

add_library(log.c
	STATIC
	../log.c/src/log.c ../log.c/src/log.h)

target_include_directories(log.c
	PUBLIC
	../log.c/src)
