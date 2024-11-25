#include "MemoryPool.h"

#include <iostream>
#include <exception>
#include <cstdarg>


// --------------------------- PRIVATE ---------------------------

void MemoryPool::init(MemoryPool::pocket& p) const
{
    p.m_totalAllocatedUnits = 0;
    p.m_totalAvailableBytes = m_allocationBytes;
    p.m_pAllocatedMemBlock = malloc(m_allocationBytes);

    if (p.m_pAllocatedMemBlock == nullptr)
    {
        std::cerr << "Could not allocate " << m_allocationBytes << " bytes of memory.\n";
        throw std::bad_alloc();
    }
}

std::ofstream& MemoryPool::memoryAllocationLogger()
{
    static std::ofstream logger("allocations.log", std::ios::out);
    return logger;
}

inline void MemoryPool::logMessage(const char* format...)
{
    if constexpr (MemoryPool::MEMORY_LOGGING_ENABLED)
    {
        static char LOG_BUFFER[BUFSIZ];

        va_list args;
        va_start(args, format);

        std::vsnprintf(LOG_BUFFER, BUFSIZ, format, args);

        va_end(args);

        memoryAllocationLogger() << LOG_BUFFER;
    }
}

// --------------------------- PUBLIC ---------------------------

MemoryPool::MemoryPool(size_t inst_bytes, size_t alloc_bytes)
    : m_unitByteSize(inst_bytes),
    m_allocationBytes(alloc_bytes)
{
    m_allocatedPockets.reserve(50);
    m_allocatedPockets.push_back({});

    init(m_allocatedPockets[0]);

    m_pocketIndex = 0;

    m_currentPocket = &m_allocatedPockets[0];
}

MemoryPool::~MemoryPool()
{
    logMessage("Cleaning, 0x%p\n", reinterpret_cast<void*>(this));

    for (pocket& p : m_allocatedPockets) {
        free(p.m_pAllocatedMemBlock);
    }
}

void* MemoryPool::allocate()
{
    if (m_currentPocket->m_totalAvailableBytes < m_unitByteSize)
    {
        m_allocatedPockets.push_back({});
        init(m_allocatedPockets[++m_pocketIndex]);
        m_currentPocket = &m_allocatedPockets[m_pocketIndex];
    }

    logMessage("\nPocket [-0x%p-] (%d) has %zu space left in bytes\n",
        m_currentPocket,
        m_pocketIndex,
        m_currentPocket->m_totalAvailableBytes
    );
    logMessage("\t > ALLOCATING %zu bytes: ", m_unitByteSize);

    if (m_pocketsWithGaps.empty())
    {
        char* temp = reinterpret_cast<char*>(m_currentPocket->m_pAllocatedMemBlock);

        logMessage("No gaps found to fill, continuing expansion\n");

        m_currentPocket->m_totalAvailableBytes -= m_unitByteSize;

        return reinterpret_cast<void*>(temp + m_unitByteSize * (m_currentPocket->m_totalAllocatedUnits++));
    }
    size_t at = m_pocketsWithGaps.begin()->first;
    void* freeSpace = m_allocatedPockets[at].m_gapsInPocket.front();

    m_allocatedPockets[at].m_gapsInPocket.pop();

    logMessage("Gap at  (0x%p), pocket: 0x%p. Filling the gap (%zu) left\n",
        freeSpace,
        &m_allocatedPockets[at],
        m_allocatedPockets[at].m_gapsInPocket.size()
    );
    m_pocketsWithGaps[at] -= 1;
    m_allocatedPockets[at].m_totalAvailableBytes -= m_unitByteSize;

    if (m_pocketsWithGaps[at] == 0) {
        m_pocketsWithGaps.erase(at);
    }

    return freeSpace;
}

void MemoryPool::deallocate(void* mem)
{
    bool b1;
    bool b2;

    for (auto i = m_pocketIndex; i >= 0; --i)
    {
        b1 = m_allocatedPockets[i].m_pAllocatedMemBlock <= mem;
        b2 = mem < reinterpret_cast<void*>(reinterpret_cast<char*>(m_allocatedPockets[i].m_pAllocatedMemBlock) + m_allocationBytes);

        if (b1 && b2)
        {
            m_allocatedPockets[i].m_totalAvailableBytes += m_unitByteSize;

            logMessage("\t > RELEASING  %zu bytes: Pushing (0x%p) to stack. Pocket: 0x%p (%zu current)\n",
                m_unitByteSize,
                mem,
                &m_allocatedPockets[i],
                m_allocatedPockets[i].m_gapsInPocket.size() + 1
            );

            m_pocketsWithGaps[static_cast<size_t>(i)] += 1;
            m_allocatedPockets[i].m_gapsInPocket.push(mem);
            return;
        }
    }
    throw std::exception();
}
