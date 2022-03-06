#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

typedef enum TestResultState {
    TestResultState_Unknown,
    TestResultState_Pass,
    TestResultState_Fail,
} TestResultState;

typedef struct TestResult {
    TestResultState State;
} TestResult;

#ifdef __cplusplus
}
#endif
