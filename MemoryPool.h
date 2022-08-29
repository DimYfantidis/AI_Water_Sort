#pragma once

#include <queue>

#define GIGABYTE    1073741824
#define GIGABYTE_2  2147483648
#define ALLOCATE    GIGABYTE


class MemoryPool
{
private:
    void* m_pAllocatedMemBlock;

    size_t m_totalAllocatedUnits;

    const size_t m_unitByteSize;

    size_t m_totalAvailableBytes;

    std::queue<void *> m_gapsInMemBlock;

    MemoryPool *m_nextMemoryPool;

public:
    MemoryPool() = delete;

    explicit MemoryPool(size_t bytes);

    ~MemoryPool();

    void* allocate();

    void deallocate(void* mem);
};
