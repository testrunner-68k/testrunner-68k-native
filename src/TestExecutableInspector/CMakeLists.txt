
add_executable(TestExecutableInspector
	TestExecutableInspector.c gettimeofday.c)

target_link_libraries(TestExecutableInspector
	PUBLIC
	LRUCachedFile HunkFileParser)
