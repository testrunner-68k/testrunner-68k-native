
add_executable(minimal_c
	hello.c ../test_framework.h)

target_link_libraries(minimal_c
	PUBLIC
	EmbeddedTestFramework)
