#include "MemoryPool.h"
#include "logging.h"

#include <iostream>
#include <exception>


// --------------------------- PRIVATE ---------------------------

void MemoryPool::init(MemoryPool::pocket &p) const
{
    p.m_totalAllocatedUnits = 0;
    p.m_totalAvailableBytes = m_allocationBytes;
    p.m_pAllocatedMemBlock  = malloc(m_allocationBytes);

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
    :   m_unitByteSize(inst_bytes),
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
#if MEMORY_LOGGING_ENABLED
    memoryAllocationLogger() << "Cleaning " << reinterpret_cast<void *>(this) << '\n';
#endif
    for (pocket &p : m_allocatedPockets) {
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

#if MEMORY_LOGGING_ENABLED
    memoryAllocationLogger() << "\nPocket [-" << reinterpret_cast<void *>(m_currentPocket) << "-] (" << m_pocketIndex
                             << ") has " << m_currentPocket->m_totalAvailableBytes << " space left in bytes\n"
                             << "\t> ALLOCATING " << m_unitByteSize << " bytes: ";
#endif

    if (m_pocketsWithGaps.empty())
    {
        char *temp = reinterpret_cast<char *>(m_currentPocket->m_pAllocatedMemBlock);
#if MEMORY_LOGGING_ENABLED
        memoryAllocationLogger() << "No gaps found to fill, continuing expansion\n";
#endif
        m_currentPocket->m_totalAvailableBytes -= m_unitByteSize;

        return reinterpret_cast<void *>(temp + m_unitByteSize * (m_currentPocket->m_totalAllocatedUnits++));
    }
    size_t at = m_pocketsWithGaps.begin()->first;
    void *freeSpace = m_allocatedPockets[at].m_gapsInPocket.front();

    m_allocatedPockets[at].m_gapsInPocket.pop();

#if MEMORY_LOGGING_ENABLED
    memoryAllocationLogger() << "Gap at  (" << freeSpace << "), pocket: "
                             << reinterpret_cast<void *>(&m_allocatedPockets[at]) << ". Filling the gap ("
                             << m_allocatedPockets[at].m_gapsInPocket.size() << " left)\n";
#endif
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
        b2 = mem < reinterpret_cast<void *>(reinterpret_cast<char *>(m_allocatedPockets[i].m_pAllocatedMemBlock) + m_allocationBytes);

        if (b1 && b2)
        {
            m_allocatedPockets[i].m_totalAvailableBytes += m_unitByteSize;
#if MEMORY_LOGGING_ENABLED
            memoryAllocationLogger() << "\t> RELEASING  " << m_unitByteSize << " bytes: Pushing (" << mem
                                     << ") to stack. Pocket: " << reinterpret_cast<void *>(&m_allocatedPockets[i])
                                     << " (" << m_allocatedPockets[i].m_gapsInPocket.size() + 1 << " current)\n";
#endif
            m_pocketsWithGaps[static_cast<size_t>(i)] += 1;
            m_allocatedPockets[i].m_gapsInPocket.push(mem);
            return;
        }
    }
    throw std::exception();
}
