#pragma once

#include <fstream>

#define MEMORY_LOGGING_ENABLED false


std::ofstream& memoryAllocationLogger()
{
    static std::ofstream logger("allocations.log", std::ios::out);
    return logger;
}