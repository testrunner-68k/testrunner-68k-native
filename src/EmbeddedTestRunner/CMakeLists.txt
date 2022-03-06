
set(ASM_SOURCES
  EmbeddedTestRunnerAsm.s)

set(C_SOURCES
  EmbeddedTestRunner.c EmbeddedTestRunner.h
  EmbeddedTestRunnerAsm.h)

set_source_files_properties(${ASM_SOURCES} PROPERTIES LANGUAGE AMIGA_VASM)

add_library(EmbeddedTestRunner
    STATIC
	${C_SOURCES} ${ASM_SOURCES})

target_include_directories(EmbeddedTestRunner
	PUBLIC
	.)

target_link_libraries(EmbeddedTestRunner
	PUBLIC
	blah)
