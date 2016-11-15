#ifndef HAVE_UTIL_HPP
#define HAVE_UTIL_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <limits>
#include <type_traits>

#include <iostream>
#include <iomanip>

#include "exception.hpp"

uint8_t vector_to_byte(std::vector< uint8_t >&);
std::vector< uint8_t > byte_to_vector(uint8_t);

uint16_t byte_swap(uint16_t);
int16_t byte_swap(int16_t);
uint32_t byte_swap(uint32_t);
int32_t byte_swap(int32_t);
uint64_t byte_swap(uint64_t);
int64_t byte_swap(int64_t);

inline uint16_t big_to_little(uint16_t v) { return byte_swap(v); }
inline uint16_t little_to_big(uint16_t v) { return byte_swap(v); }
inline uint32_t big_to_little(uint32_t v) { return byte_swap(v); }
inline uint32_t little_to_big(uint32_t v) { return byte_swap(v); }
inline uint64_t big_to_little(uint64_t v) { return byte_swap(v); }
inline uint64_t little_to_big(uint64_t v) { return byte_swap(v); }

uint16_t vector_to_word(const std::vector< uint8_t >&);
std::vector< uint8_t > word_to_vector(const uint16_t);

uint32_t vector_to_dword(const std::vector< uint8_t >&);
std::vector< uint8_t > dword_to_vector(const uint32_t);

uint64_t vector_to_qword(const std::vector< uint8_t >&);
std::vector< uint8_t > qword_to_vector(const uint64_t);

uint64_t reverse(uint64_t, uint64_t);

template <typename T>
T
reverse_bits(const T val, const uint64_t b = std::numeric_limits< T >::digits)
{
	T n = val;
	T rv = 0;
	uint64_t dig(std::numeric_limits< T >::digits);
	
	if ( dig < b )
		throw std::exception("reverse_bits(): Invalid bitcount; exceeds number of possible bits.");

	for ( size_t i = 0; i < b; ++i, n >>= 1 )
		rv = ( rv << 1 ) + ( n & 0x01 );

	return rv;
}

template< typename T >
T
inverse_bits(const T val, const uint64_t b = std::numeric_limits< T >::digits)
{
	T n(val);
	uint64_t dig(std::numeric_limits< T >::digits);

	if ( std::numeric_limits< T >::digits < b )
		throw std::exception("inverse_bits(): Invalid bitcount; exceeds number of possible bits.");

	return ~(n) & b;
}

#endif



