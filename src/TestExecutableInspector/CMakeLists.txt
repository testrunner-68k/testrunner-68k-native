
add_executable(TestExecutableInspector
	TestExecutableInspector.c)

target_link_libraries(TestExecutableInspector
	PUBLIC
	HunkFileParser)
