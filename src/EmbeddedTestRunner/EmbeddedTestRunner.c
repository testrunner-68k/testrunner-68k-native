#include "EmbeddedTestRunner.h"
#include "EmbeddedTestRunnerAsm.h"
#include "HunkFileParser.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

#include <proto/exec.h>
#include <dos/dosextens.h>
#include <exec/tasks.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool EmbeddedTestRunner_runTests(int numTests, Test* tests)
{
    bool testsSuccess = true;
    for (int testId = 0; testId < numTests; testId++) {
        printf("Running test %d - %s: ", testId, tests[testId].Name);
        const bool testSuccess = EmbeddedTestRunner_runTest(&tests[testId]);
        testsSuccess &= testSuccess;
        printf("%s\n", testSuccess ? "PASS" : "FAIL");
    }

    return testsSuccess;
}

typedef struct 
{
    BPTR Next;
    uint8_t Data[];
} LoadedSegmentHeader;

bool EmbeddedTestRunner_getLoadedSegments(uint8_t*** loadedSegments, int* numLoadedSegments)
{
    // Locate Process struct for currently-running program

    struct Process* process = (struct Process*) FindTask(0);
    if (!process) {
        log_error("FindTask(0) failed");
        return false;
    }

    // Locate SegList for process

    LoadedSegmentHeader* segList = 0;

    /* Reference: Amiga Guru Book Pages: 538ff,565,573
		     and XOper.asm */
	if (process->pr_Task.tc_Node.ln_Type==NT_PROCESS) {
	  if (process->pr_CLI == 0) {
	    segList = (LoadedSegmentHeader*) BADDR(process->pr_SegList);
	  } else {
	    /* ProcLoaded	'Loaded as a command: '*/
	    struct CommandLineInterface* cli = (struct CommandLineInterface*) (BADDR(process->pr_CLI));
	    segList = (LoadedSegmentHeader*) BADDR(cli->cli_Module);
	  }
	} else {
	  log_error("Current task node is not AmigaDOS Process");
      return false;
 	}

	if (!segList) {
	  log_error("Cannot find SegList within process descriptor");
      return false;
 	}

    // Count number of segments in SegList

    int numSegments = 0;

    {
        LoadedSegmentHeader* segment = segList;

        for (numSegments = 0; segment->Next; (numSegments++))
            segment = (LoadedSegmentHeader*) BADDR(segment->Next);
    }

    // Create pointer-array to each segment

    uint8_t** segments = (uint8_t**) malloc(numSegments * sizeof(uint8_t*));

    {
        LoadedSegmentHeader* segment = segList;

        for (int segmentIndex = 0; segmentIndex < numSegments; segmentIndex++) {
            segments[segmentIndex] = segment->Data;
            segment = (LoadedSegmentHeader*) BADDR(segment->Next);
        }
    }

    *loadedSegments = segments;
    *numLoadedSegments = numSegments;
    return true;
}

TestEntryPoint EmbeddedTestRunner_getTestEntryPoint(Test* test)
{
    uint8_t** loadedSegments;
    int numLoadedSegments;

    if (!EmbeddedTestRunner_getLoadedSegments(&loadedSegments, &numLoadedSegments)) {
        return 0;
    }

    if (test->Hunk >= numLoadedSegments) {
        log_error("Test %s is in hunk %d, but there are only %d loaded segments", test->Name, test->Hunk, numLoadedSegments);
        return 0;
    }

    TestEntryPoint testEntryPoint = (TestEntryPoint) (loadedSegments[test->Hunk] + test->Offset);

    log_debug("Test entry point: hunk %d, offset %d => absolute address %08x", test->Hunk, test->Offset, testEntryPoint);

    return testEntryPoint;
}

bool EmbeddedTestRunner_runTest(Test* test)
{
    TestEntryPoint testEntryPoint = EmbeddedTestRunner_getTestEntryPoint(test);
    if (!testEntryPoint) {
        log_error("Unable to get test entry point for %s", test->Name);
        return false;
    }

    return EmbeddedTestRunner_runTestAtAddress(testEntryPoint);
}
