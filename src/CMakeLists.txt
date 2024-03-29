cmake_minimum_required(VERSION 3.10)

project(testrunner_68k_native
	LANGUAGES C CXX)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};../src/amiga-utils/utilslib/CMake") 
enable_language(AMIGA_VASM)

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
add_subdirectory(HunkFileParser)
add_subdirectory(TestExecutableInspector)
if(EMBED_TEST_FRAMEWORK)
	add_subdirectory(EmbeddedTestFramework)
	add_subdirectory(EmbeddedTestRunner)
	add_subdirectory(examples/minimal_c)
endif()


# log.c

add_library(log.c
	STATIC
	log.c/src/log.c log.c/src/log.h)

target_include_directories(log.c
	PUBLIC
	log.c/src)
