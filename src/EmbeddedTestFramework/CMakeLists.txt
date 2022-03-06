add_library(EmbeddedTestFramework
    STATIC
	test_main.c test_main.h)

target_include_directories(EmbeddedTestFramework
	PUBLIC
	.)

target_link_libraries(EmbeddedTestFramework
	PUBLIC
	HunkFileParser EmbeddedTestRunner)
