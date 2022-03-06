
set(C_SOURCES
  Endian.c Endian.h
  LinearAllocator.c LinearAllocator.h)

set(CXX_TESTS
  Endian_test.cpp
  LinearAllocator_test.cpp)

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

