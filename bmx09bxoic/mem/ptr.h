#pragma once
#include <memory>
#include <algorithm>
#include <cstdint>

class Ptr
{
public:
    Ptr() : ptr(0) {}
    Ptr(uintptr_t other) : ptr(other) {}

    template<typename U>
    U as()
    {
        return reinterpret_cast<U>(ptr);
    }

    Ptr add(uintptr_t offset)
    {
        return Ptr(ptr + offset);
    }

    Ptr sub(uintptr_t offset)
    {
        return Ptr(ptr - offset);
    }

    Ptr deref()
    {
        return *reinterpret_cast<uintptr_t*>(ptr);
    }

    uintptr_t raw() const
    {
        return ptr;
    }

private:
    uintptr_t ptr;
};