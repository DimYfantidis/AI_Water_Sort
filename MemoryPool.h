#pragma once

#include <set>
#include <queue>
#include <utility>
#include <fstream>
#include <unordered_map>


class MemoryPool
{
private:
    static std::ofstream& memoryAllocationLogger();

    static constexpr bool MEMORY_LOGGING_ENABLED = false;

    static inline void logMessage(const char* format...);

private:
    const size_t m_unitByteSize;
    const size_t m_allocationBytes;

    struct pocket
    {
        void* m_pAllocatedMemBlock;

        size_t m_totalAllocatedUnits;
        size_t m_totalAvailableBytes;

        std::queue<void*> m_gapsInPocket;
    };

    pocket* m_currentPocket;

    int m_pocketIndex;

    std::vector<pocket> m_allocatedPockets;

    std::unordered_map<size_t, size_t> m_pocketsWithGaps;

    void init(pocket& p) const;

public:
    MemoryPool() = delete;

    MemoryPool(const MemoryPool&) = delete;

    MemoryPool(size_t inst_bytes, size_t alloc_bytes);

    ~MemoryPool();

    void* allocate();

    void deallocate(void* mem);
};
