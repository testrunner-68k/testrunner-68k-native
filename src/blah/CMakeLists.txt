cmake_minimum_required(VERSION 3.10)

project(Blah
	LANGUAGES C)

# blah

add_executable(blah
	test.c)

target_link_libraries(blah
	PUBLIC
	amiga_hunk_parser)

# amiga_hunk_parser

add_library(amiga_hunk_parser
	STATIC
	../AmigaHunkParser/amiga_hunk_parser.c)

target_include_directories(amiga_hunk_parser
	PUBLIC
	../AmigaHunkParser)



