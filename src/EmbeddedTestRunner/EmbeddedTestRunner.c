#include "EmbeddedTestRunner.h"
#include "EmbeddedTestRunnerAsm.h"
#include "HunkFileParser.h"
#include "LinearAllocator.h"
#include "log.h"
#include "TestResult.h"
#include <stdlib.h>
#include <stdio.h>

#include <proto/exec.h>
#include <dos/dosextens.h>
#include <exec/tasks.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool EmbeddedTestRunner_runTests(LinearAllocator* linearAllocator, int numTests, Test* tests, TestResult** testResults)
{
    TestEntryPoint* testEntryPoints;

    if (!EmbeddedTestRunner_getTestEntryPoints(linearAllocator, numTests, tests, &testEntryPoints)) {
        return false;
    }

    if (!LinearAllocator_allocate(linearAllocator, numTests * sizeof(TestResult), (uint8_t**) testResults)) {
        log_error("Unable to allocate %d test results", numTests);
        return false;
    }

    for (int testId = 0; testId < numTests; testId++) {
        (*testResults)[testId].State = TestResultState_Unknown;
    }

    for (int testId = 0; testId < numTests; testId++) {
        TestResult* testResult = &((*testResults)[testId]);
        printf("Running test %d - %s: ", testId, tests[testId].Name);
        const bool testSuccess = EmbeddedTestRunner_runTest(&tests[testId], testEntryPoints[testId]);
        testResult->State = (testSuccess ? TestResultState_Pass : TestResultState_Fail);
        printf("%s\n", testSuccess ? "PASS" : "FAIL");
    }

    return true;
}

TestResultState EmbeddedTestRunner_analyzeResults(int numTests, TestResult* testResults)
{
    if (!numTests)
        return TestResultState_Unknown;

    TestResultState result = TestResultState_Pass;

    for (int testId = 0; testId < numTests; testId++) {
        TestResult* testResult = &testResults[testId];
        if (testResult->State == TestResultState_Unknown)
            return TestResultState_Unknown;
        else if (testResult->State == TestResultState_Fail)
            result = TestResultState_Fail;
    }

    return result;
}

typedef struct 
{
    BPTR Next;
    uint8_t Data[];
} LoadedSegmentHeader;

bool EmbeddedTestRunner_getLoadedSegments(LinearAllocator* linearAllocator, uint8_t*** loadedSegments, int* numLoadedSegments)
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

    log_debug("Num loaded segments: %d", numSegments);

    // Create pointer-array to each segment

    uint8_t** segments;
    if (!LinearAllocator_allocate(linearAllocator, numSegments * sizeof(uint8_t*), (uint8_t**) &segments)) {
        log_error("Unable to allocate memory for %d segment ptrs", numSegments);
        return false;
    }

    {
        LoadedSegmentHeader* segment = segList;

        for (int segmentIndex = 0; segmentIndex < numSegments; segmentIndex++) {
            segments[segmentIndex] = segment->Data;
            segment = (LoadedSegmentHeader*) BADDR(segment->Next);
            log_debug("Segment %d begins at %08x", segmentIndex, segments[segmentIndex]);
        }
    }

    *loadedSegments = segments;
    *numLoadedSegments = numSegments;
    return true;
}

bool EmbeddedTestRunner_getTestEntryPoints(LinearAllocator* linearAllocator, int numTests, Test* tests, TestEntryPoint** testEntryPoints) {

    uint8_t** loadedSegments;
    int numLoadedSegments;

    if (!EmbeddedTestRunner_getLoadedSegments(linearAllocator, &loadedSegments, &numLoadedSegments)) {
        return false;
    }

    if (!LinearAllocator_allocate(linearAllocator, numTests * sizeof(TestEntryPoint), (uint8_t**) testEntryPoints)) {
        log_error("Unable to allocate memory for %d test entry points", numTests);
        return false;
    }

    for (int testIndex = 0; testIndex < numTests; testIndex++) {
        Test* test = &tests[testIndex];
        if (test->Hunk >= numLoadedSegments) {
            log_error("Test %s is in hunk %d, but there are only %d loaded segments", test->Name, test->Hunk, numLoadedSegments);
            return false;
        }

        TestEntryPoint testEntryPoint = (TestEntryPoint) (loadedSegments[test->Hunk] + test->Offset);

        log_debug("Test %s entry point: hunk %d, offset %d => absolute address %08x", test->Name, test->Hunk, test->Offset, testEntryPoint);

        (*testEntryPoints)[testIndex] = testEntryPoint;
    }

    return true;
}

bool EmbeddedTestRunner_runTest(Test* test, TestEntryPoint testEntryPoint)
{
    return EmbeddedTestRunner_runTestAtAddress(testEntryPoint);
}
