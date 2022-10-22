#pragma once
#include <iostream>
#include <assert.h>
#include "bmi2intrin.h"

#define ENABLEDBG 0
#define ENABLEPRINT 0

#define IFDBG if constexpr (ENABLEDBG) 
#define IFPRN if constexpr (ENABLEPRINT) 

#define m_assert(expr, msg) assert(( (void)(msg), (expr) ))

typedef uint64_t BBoard;
typedef uint64_t Square;
typedef uint64_t Bit;

//macro for looping over bits in BBoard
#define LoopBits for(;X; X = _blsr_u64(X))

//returns the lowest active bit in the U64, use for Square/Bit
#define LowestOne _tzcnt_u64(X)

#define BitCount(X) static_cast<uint64_t>(__builtin_popcountll(X))

#define _ASSUME(cond) ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#define COMPILETIME __attribute__((always_inline)) static constexpr
#define NOINL __attribute__ ((noinline))
#define INL inline
#define FORCEINL __attribute__((always_inline)) inline


inline static Bit PopBit(uint64_t& val)
{
    uint64_t lsb = _blsi_u64(val);
    val ^= lsb;
    return lsb;
}

