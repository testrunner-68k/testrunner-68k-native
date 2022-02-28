
add_executable(TestExecutableInspector
	TestExecutableInspector.c)

target_link_libraries(TestExecutableInspector
	PUBLIC
	LRUCachedFile HunkFileParser)
