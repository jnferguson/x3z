#include "stdafx.h"
#include "util.hpp"
#include "exception.hpp"

uint64_t
reverse(uint64_t val, uint64_t len)
{
	if ( 8 >= len )
		return reverse_bits< uint8_t >(static_cast< uint8_t >( val & UINT8_MAX ), len) & UINT8_MAX;
	else if ( 16 >= len )
		return reverse_bits< uint16_t >(static_cast< uint16_t >( val & UINT16_MAX )) & UINT16_MAX;
	else if ( 32 >= len )
		return reverse_bits< uint32_t >(static_cast< uint32_t >( val & UINT32_MAX )) & UINT32_MAX;
	else if ( 64 >= len )
		return reverse_bits< uint64_t >(val) & UINT64_MAX;
	else
		throw std::exception("reverse(): Invalid bit-length encountered");

	return 0;
}

uint16_t
byte_swap(uint16_t v)
{
	return static_cast< uint16_t >( ( v << 8 ) | ( v >> 8 ) );
}

int16_t
byte_swap(int16_t v)
{
	return static_cast< int16_t >( ( v << 8 ) | ( ( v >> 8 ) & 0xFF ) );
}

uint32_t
byte_swap(uint32_t v)
{
	uint32_t val(( ( v << 8 ) & 0xFF00FF00 ) | ( ( v >> 8 ) & 0x00FF00FF ));
	return static_cast< uint32_t >( ( val << 16 ) | ( val >> 16 ) );
}

int32_t
byte_swap(int32_t v)
{
	int32_t val(( ( v << 8 ) & 0xFF00FF00 ) | ( ( v >> 8 ) & 0x00FF00FF ));
	return static_cast< int32_t >( ( val << 16 ) | ( ( val >> 16 ) & 0xFFFF ) );
}

uint64_t
byte_swap(uint64_t v)
{
	uint64_t val(( ( v << 8 ) & 0xFF00FF00FF00FF00ULL ) | ( ( v >> 8 ) & 0x00FF00FF00FF00FFULL ));

	val = ( ( val << 16 ) & 0xFFFF0000FFFF0000ULL ) | ( ( val >> 16 ) & 0x0000FFFF0000FFFFULL );
	return static_cast< uint64_t >( ( val << 32 ) | ( ( val >> 32 ) ) );
}

int64_t
byte_swap(int64_t v)
{
	int64_t val(( ( v << 8 ) & 0xFF00FF00FF00FF00ULL ) | ( ( v >> 8 ) & 0x00FF00FF00FF00FFULL ));
	val = ( ( val << 16 ) & 0xFFFF0000FFFF0000ULL ) | ( ( val >> 16 ) & 0x0000FFFF0000FFFFULL );
	return static_cast< int64_t >( ( val << 32 ) | ( ( val >> 32 ) & 0x00000000FFFFFFFFULL ) );
}

uint8_t
vector_to_byte(const std::vector< uint8_t >& bv)
{
	uint8_t ret(0);

	if ( sizeof(uint8_t) != bv.size() )
		throw std::exception("vector_to_byte(): Invalid parameter specified");

	ret = bv.at(0);
	return ret;
}

std::vector< uint8_t >
byte_to_vector(const uint8_t b)
{
	std::vector< uint8_t > ret;

	ret.push_back(b);
	return ret;
}

uint16_t
vector_to_word(const std::vector< uint8_t >& bv)
{
	uint16_t ret(0);

	if ( sizeof(uint16_t) != bv.size() )
		throw std::exception("vector_to_word(): Invalid parameter specified");

	std::memcpy(&ret, bv.data(), sizeof(uint16_t));
	return big_to_little(ret);
}

std::vector< uint8_t >
word_to_vector(const uint16_t w)
{
	std::vector< uint8_t >  ret;
	uint16_t                val(little_to_big(w));

	ret.resize(sizeof(uint16_t));
	std::memcpy(ret.data(), &val, sizeof(uint16_t));
	return ret;
}

uint32_t
vector_to_dword(const std::vector< uint8_t >& bv)
{
	uint32_t ret(0);

	if ( sizeof(uint32_t) != bv.size() )
		throw std::exception("vector_to_dword(): Invalid parameter specified");

	std::memcpy(&ret, bv.data(), sizeof(uint32_t));
	return big_to_little(ret);
}

std::vector< uint8_t >
dword_to_vector(const uint32_t d)
{
	std::vector< uint8_t >  ret;
	uint32_t                val(little_to_big(d));

	ret.resize(sizeof(uint32_t));
	std::memcpy(ret.data(), &val, sizeof(uint32_t));
	return ret;
}

uint64_t
vector_to_qword(const std::vector< uint8_t >& bv)
{
	uint64_t ret(0);

	if ( sizeof(uint64_t) != bv.size() )
		throw std::exception("vector_to_qword(): Invalid parameter specified");

	std::memcpy(&ret, bv.data(), sizeof(uint64_t));
	return big_to_little(ret);
}

std::vector< uint8_t >
qword_to_vector(const uint64_t q)
{
	std::vector< uint8_t >  ret;
	uint64_t                val(little_to_big(q));

	ret.resize(sizeof(uint64_t));
	std::memcpy(ret.data(), &val, sizeof(uint64_t));

	return ret;
}

