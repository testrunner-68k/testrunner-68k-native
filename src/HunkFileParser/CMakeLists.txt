
set(C_SOURCES
  HunkFileParser.c HunkFileParser.h
  LRUCachedFile.c LRUCachedFile.h
  LRUCachedFileReader.c LRUCachedFileReader.h
  TestDescriptor.h
  )

set(CXX_TESTS
  HunkFileParser_test.cpp
  LRUCachedFile_test.cpp
  LRUCachedFileReader_test.cpp
  )

add_library(HunkFileParser
	STATIC
	${C_SOURCES})

target_include_directories(HunkFileParser
	PUBLIC
	.)

target_link_libraries(
  HunkFileParser
  blah log.c
)

if(INCLUDE_TESTS)
  add_executable(
    HunkFileParser_test
    ${CXX_TESTS}
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
endif()
