#pragma once

#include <set>
#include <queue>
#include <utility>
#include <fstream>
#include <unordered_map>


class MemoryPool
{
private:
    const size_t m_unitByteSize;
    const size_t m_allocationBytes;

    struct pocket
    {
        void* m_pAllocatedMemBlock = nullptr;

        size_t m_totalAllocatedUnits = 0;
        size_t m_totalAvailableBytes = 0;

        std::queue<void*> m_gapsInPocket;
    };

    pocket* m_currentPocket;

    int m_pocketIndex;

    std::vector<pocket> m_allocatedPockets;

    std::unordered_map<size_t, size_t> m_pocketsWithGaps;

    void init(pocket& p) const;

    static std::ofstream& memoryAllocationLogger();

public:
    MemoryPool() = delete;

    MemoryPool(const MemoryPool&) = delete;

    MemoryPool(size_t inst_bytes, size_t alloc_bytes);

    ~MemoryPool();

    void* allocate();

    void deallocate(void* mem);
};
