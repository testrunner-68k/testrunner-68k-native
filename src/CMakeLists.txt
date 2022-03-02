cmake_minimum_required(VERSION 3.10)

project(testrunner_68k_native
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)

# GoogleTest requires at least C++11 -- but we'll go for C++17
set(CMAKE_CXX_STANDARD 17)

if(INCLUDE_TESTS)
	include(FetchContent)
	FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)

	enable_testing()
endif()



add_subdirectory(blah)
add_subdirectory(TestExecutableInspector)