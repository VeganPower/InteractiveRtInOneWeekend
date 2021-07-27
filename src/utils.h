#pragma once
#include "types.h"
#ifndef ENABLE_ASSERTS
    #ifdef NDEBUG
    #define ENABLE_ASSERTS 0
    #else
    #define ENABLE_ASSERTS 1
    #endif
#endif

#if ENABLE_ASSERTS
#include <windows.h>
#endif

struct NotCopyable
{
   NotCopyable() = default;
   NotCopyable(const NotCopyable& ) = delete;
   NotCopyable& operator=(NotCopyable const& to_assign) = delete;
};

struct NotMovable
{
   NotMovable() = default;
   NotMovable(NotMovable&& ) = delete;
   NotMovable& operator=(NotMovable&& to_assign) = delete;
};

template<typename T>
void set_to_zero(T& data)
{
    memset(&data, 0, sizeof(T));
}

template<typename T>
constexpr T min_of(T const& a, T const& b)
{
   return a < b ? a : b;
}

template<typename T>
constexpr T max_of(T const& a, T const& b)
{
   return a > b ? a : b;
}

template<typename T>
constexpr T clamp(T const& a, T const& low, T const& high)
{
   return max_of(low, min_of(high, a));
}

template<typename T>
constexpr T saturate(T x)
{
    return max_of(T{0}, min_of(T{1}, x));
}

constexpr u8 extract_bits(u8 source, u32 shift, u32 size)
{
   u8 mask = (1 << size) - 1;
   u8 temp = source >> shift;
   return temp & mask;
}

constexpr u16 extract_bits(u16 source, u32 shift, u32 size)
{
   u16 mask = (1 << size) - 1;
   u16 temp = source >> shift;
   return temp & mask;
}

constexpr u32 extract_bits(u32 source, u32 shift, u32 size)
{
   u32 mask = (1 << size) - 1;
   u32 temp = source >> shift;
   return temp & mask;
}

constexpr u64 extract_bits(u64 source, u32 shift, u32 size)
{
   u64 mask = (1 << size) - 1;
   u64 temp = source >> shift;
   return temp & mask;
}

#if ENABLE_ASSERTS
constexpr void check_predicate(const char* file_name, int line_number, const char* name, bool predicate)
{
    if (!predicate)
    {
        OutputDebugString("Assertiong failed\n");
        DebugBreak();
    }
}

#define ASSERT(predicate) check_predicate(__FILE__, __LINE__, #predicate, predicate)
#else
#define ASSERT(predicate)
#endif