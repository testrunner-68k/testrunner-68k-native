#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct LinearAllocator {
    uint8_t* Buffer;
    int Size;
    int AllocationOffset;
} LinearAllocator;

void LinearAllocator_init(LinearAllocator* linearAllocator, uint8_t* buffer, int size);

bool LinearAllocator_allocate(LinearAllocator* linearAllocator, int size, uint8_t** buffer);

#ifdef __cplusplus
}
#endif
