#pragma once
#include <iostream>
#include <assert.h>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>

#include <immintrin.h>

#define ENABLEDBG 1
#define ENABLEPRINT 0

#define IFDBG if constexpr (ENABLEDBG) 
#define IFPRN if constexpr (ENABLEPRINT) 

#define m_assert(expr, msg) assert(( (void)(msg), (expr) ))

typedef uint64_t BBoard;
typedef uint64_t Square;
typedef uint64_t Bit;

//macro for looping over bits in BBoard
#define LoopBits(X) for(;X; X = _blsr_u64(X))

//returns the lowest active bit in the U64, use for Square/Bit, this will be reduced in the for loop in LoopBits, thus you want to operate on it right aways
#define LSquare(X) _tzcnt_u64(X)

#define BitCount(X) static_cast<uint64_t>(__builtin_popcountll(X))

#define _ASSUME(cond) ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#define COMPILETIME __attribute__((always_inline)) static constexpr
#define NOINL __attribute__ ((noinline))
#define INL inline
#define FORCEINL __attribute__((always_inline)) inline


inline unsigned long LeastBit(const uint64_t &v)
{
    return __builtin_ctzll(v);
}
inline static Bit PopBit(uint64_t& val)
{
    uint64_t lsb = _blsi_u64(val);
    val ^= lsb;
    return lsb;
}

static const std::string notations[] =
{ 
  "A1","B1","C1","D1","E1","F1","G1","H1",
  "A2","B2","C2","D2","E2","F2","G2","H2",
  "A3","B3","C3","D3","E3","F3","G3","H3",
  "A4","B4","C4","D4","E4","F4","G4","H4",
  "A5","B5","C5","D5","E5","F5","G5","H5",
  "A6","B6","C6","D6","E6","F6","G6","H6",
  "A7","B7","C7","D7","E7","F7","G7","H7",
  "A8","B8","C8","D8","E8","F8","G8","H8"
};

static int notation_idx(std::string notation)
{
    std::transform(notation.begin(), notation.end(),notation.begin(), toupper);

    auto element = std::find(std::begin(notations), std::end(notations), notation);

    if (element != std::end(notations))  return std::distance(notations, element);
    else return -1;
}

template <typename T>
void print(const T & v)
{
    std::cout << v << std::endl;
}

class Timer
{
public :
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

    Timer() : time_(clock_t::now())
    {

    }

    double elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - time_).count();
    }

    void reset()
    {
        time_ = clock_t::now();
    }
private:
    std::chrono::time_point<clock_t> time_;

};