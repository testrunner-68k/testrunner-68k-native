#include "LinearAllocator.h"
#include "log.h"

enum {
    LinearAllocator_AllocationGranularity = 4
};

void LinearAllocator_init(LinearAllocator* linearAllocator, uint8_t* buffer, int size)
{
    linearAllocator->Buffer = buffer;
    linearAllocator->Size = size;
    linearAllocator->AllocationOffset = 0;
}

bool LinearAllocator_allocate(LinearAllocator* linearAllocator, int size, uint8_t** buffer)
{
    const int roundedUpSize = (size + (LinearAllocator_AllocationGranularity - 1)) & -LinearAllocator_AllocationGranularity;

    int newAllocationOffset = linearAllocator->AllocationOffset + roundedUpSize;

    if (newAllocationOffset > linearAllocator->Size) {
        log_error("Unable to allocate %d bytes from linear allocator", roundedUpSize);
        return false;
    } else {
        *buffer = linearAllocator->Buffer + linearAllocator->AllocationOffset;
        linearAllocator->AllocationOffset = newAllocationOffset;
        log_debug("Allocated %d bytes from linear allocator", roundedUpSize);
        return true;
    }
}
