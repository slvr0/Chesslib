#pragma once

#include <iostream>
#include <bitset>

//returns the number of ones in a bitarray
size_t popcount_64(const uint64_t v) {
  return std::bitset<64> (v).count();
}

//returns a uint64t with bits set to 1 on all positions listed in the initalizer lists integer values
uint64_t uint64t_frombits(std::initializer_list<uint8_t> vs) {
  uint64_t u64 = 0x0;

  for(auto v : vs) u64 |= 1ULL << v;

  return u64;
}