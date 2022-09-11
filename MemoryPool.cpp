#include "MemoryPool.h"

#include <iostream>
#include <exception>

#define MEMORY_LOGGING_ENABLED true

#if MEMORY_LOGGING_ENABLED
    char LOG_BUFFER[BUFSIZ];

    // Log raw string.
    #define LOG_R(message)     memoryAllocationLogger() << (message);
    // Log formatted string.
    #define LOG_F(format, ...) std::snprintf(LOG_BUFFER, BUFSIZ, format, __VA_ARGS__); memoryAllocationLogger() << LOG_BUFFER;
#else
    #define LOG_R(message)      /* do nothing */
    #define LOG_F(format, ...)  /* do nothing */
#endif // MEMORY_LOGGING_ENABLED


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
    LOG_F("Cleaning, 0x%p\n", reinterpret_cast<void*>(this))

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

    LOG_F("\nPocket [-0x%p-] (%d) has %zu space left in bytes\n",
          m_currentPocket,
          m_pocketIndex,
          m_currentPocket->m_totalAvailableBytes
    )
    LOG_F("\t > ALLOCATING %zu bytes: ", m_unitByteSize)

    if (m_pocketsWithGaps.empty())
    {
        char* temp = reinterpret_cast<char*>(m_currentPocket->m_pAllocatedMemBlock);

        LOG_R("No gaps found to fill, continuing expansion\n")

        m_currentPocket->m_totalAvailableBytes -= m_unitByteSize;

        return reinterpret_cast<void*>(temp + m_unitByteSize * (m_currentPocket->m_totalAllocatedUnits++));
    }
    size_t at = m_pocketsWithGaps.begin()->first;
    void* freeSpace = m_allocatedPockets[at].m_gapsInPocket.front();

    m_allocatedPockets[at].m_gapsInPocket.pop();

    LOG_F("Gap at  (0x%p), pocket: 0x%p. Filling the gap (%zu) left\n",
          freeSpace,
          &m_allocatedPockets[at],
          m_allocatedPockets[at].m_gapsInPocket.size()
    )
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

            LOG_F("\t > RELEASING  %zu bytes: Pushing (0x%p) to stack. Pocket: 0x%p (%zu current)\n",
                  m_unitByteSize,
                  mem,
                  &m_allocatedPockets[i],
                m_allocatedPockets[i].m_gapsInPocket.size() + 1
            )

            m_pocketsWithGaps[static_cast<size_t>(i)] += 1;
            m_allocatedPockets[i].m_gapsInPocket.push(mem);
            return;
        }
    }
    throw std::exception();
}
