#include "MemoryPool.h"
#include "logging.h"

#include <iostream>
#include <exception>


MemoryPool::MemoryPool(const size_t bytes) : m_unitByteSize(bytes)
{
    m_totalAllocatedUnits   =   0;
    m_totalAvailableBytes   =   ALLOCATE;
    m_pAllocatedMemBlock    =   malloc(m_totalAvailableBytes);
    m_nextMemoryPool        =   nullptr;

    if (m_pAllocatedMemBlock == nullptr)
    {
        std::cerr << "Could not allocate " << ALLOCATE << " bytes of memory.\n";
        throw std::bad_alloc();
    }
}

MemoryPool::~MemoryPool()
{
#if MEMORY_LOGGING_ENABLED
    memoryAllocationLogger() << "Cleaning " << reinterpret_cast<void *>(this) << '\n';
#endif

    if (m_nextMemoryPool != nullptr)
    {
        delete m_nextMemoryPool;
        m_nextMemoryPool = nullptr;
    }
    free(m_pAllocatedMemBlock);
}

void* MemoryPool::allocate()
{
    if (m_totalAvailableBytes < m_unitByteSize)
    {
        if (m_nextMemoryPool == nullptr) {
            m_nextMemoryPool = ::new MemoryPool(m_unitByteSize);
        }
        return m_nextMemoryPool->allocate();
    }

#if MEMORY_LOGGING_ENABLED
    memoryAllocationLogger() << "\nPool [-" << reinterpret_cast<void *>(this) << "-] has " << m_totalAvailableBytes << " space left in bytes" << std::endl;

    memoryAllocationLogger() << "\t> ALLOCATING " << m_unitByteSize << " bytes: ";
#endif

    if (m_gapsInMemBlock.empty())
    {
        static char *temp = reinterpret_cast<char *>(m_pAllocatedMemBlock);

#if MEMORY_LOGGING_ENABLED
    memoryAllocationLogger() << "No gaps found to fill, continuing expansion\n";
#endif
        m_totalAvailableBytes -= m_unitByteSize;

        return reinterpret_cast<void *>(temp + m_unitByteSize * (m_totalAllocatedUnits++));
    }

    void* freeSpace = m_gapsInMemBlock.front();

#if MEMORY_LOGGING_ENABLED
    memoryAllocationLogger() << "Gap at  (" << freeSpace << "). Filling the gap\n";
#endif
    m_gapsInMemBlock.pop();

    m_totalAvailableBytes -= m_unitByteSize;

    return freeSpace;
}

void MemoryPool::deallocate(void* mem)
{
    if (m_pAllocatedMemBlock < mem && mem < reinterpret_cast<void *>(reinterpret_cast<char *>(m_pAllocatedMemBlock) + ALLOCATE))
    {
        m_totalAvailableBytes += m_unitByteSize;
#if MEMORY_LOGGING_ENABLED
        memoryAllocationLogger() << "\t> RELEASING  " << m_unitByteSize << " bytes: Pushing (" << mem << ") to stack\n";
#endif
        m_gapsInMemBlock.push(mem);
    }
    else
    {
        if (m_nextMemoryPool != nullptr) {
            m_nextMemoryPool->deallocate(mem);
        }
    }
}
