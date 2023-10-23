#include <cstdio>

#include "Address.hpp"

namespace Utility {
    Address::Address()
        : m_Ptr(nullptr)
    {}

    Address::Address(void* ptr)
        : m_Ptr(ptr)
    {}

    Address::Address(uintptr_t addr)
        : m_Ptr((void*)addr)
    {}
}