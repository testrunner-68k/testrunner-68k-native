#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TestDescriptor {
    const char* Name;
    int Hunk;
    int Offset;
} TestDescriptor;

#ifdef __cplusplus
}
#endif
