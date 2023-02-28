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

#if ENABLEDBG
    #define _DEBUG
#endif


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

enum class Pieces {
    WHITEPAWN = 0 ,
    WHITEKNIGHT = 1,
    WHITEBISHOP = 2, 
    WHITEROOK = 3,
    WHITEQUEEN = 4,
    WHITEKING = 5,
    BLACKPAWN = 6,
    BLACKKNIGHT = 7,
    BLACKBISHOP = 8,
    BLACKROOK = 9,
    BLACKQUEEN = 10,
    BLACKKING = 11
};

static const std::string notations[] =
{ 
  "a1","b1","c1","d1","e1","f1","g1","h1",
  "a2","b2","c2","d2","e2","f2","g2","h2",
  "a3","b3","c3","d3","e3","f3","g3","h3",
  "a4","b4","c4","d4","e4","f4","g4","h4",
  "a5","b5","c5","d5","e5","f5","g5","h5",
  "a6","b6","c6","d6","e6","f6","g6","h6",
  "a7","b7","c7","d7","e7","f7","g7","h7",
  "a8","b8","c8","d8","e8","f8","g8","h8"
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

template <typename... Args>
inline uint64_t InsertBits(Args ... args)
{
    uint64_t ret { 0x0 };
   
    ([&] {ret |= 1ULL << args;} (), ...); 

    return ret;
}

inline std::string StringToLowerCase(const std::string& str, int capped_lower = 0) {
    std::string res = str.c_str();
    std::for_each(res.begin(), res.end(), [](char& c) // modify in-place
    {
        c = std::tolower(static_cast<unsigned char>(c));
    });


    return res;
}

inline std::vector<std::string> __SplitString(const std::string& str,
                                    const std::string& delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}
