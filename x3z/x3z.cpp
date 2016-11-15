// x3z.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>

#include "huffman.hpp"
#include "hexdump.hpp"
#include "rand.hpp"
#include "util.hpp"
#include "failed.hpp"

uint8_t bytes[] = "mississippi zriver";

signed int 
main(signed int ac, char** av)
{
	
	htree_t					ht;
	hexdump_t				hex;
	rand_t					rnd;
	uint32_t				siz(0);
	uint64_t				cnt(0);
	uint64_t				thou(0);
	std::vector< uint8_t >	src, dst;
	std::vector< uint8_t >	src_copy, dst_copy;
//	std::chrono::time_point< std::chrono::steady_clock > start, end;
//	std::chrono::duration < double, std::milli > ediff, ddiff, ed, dd;

/*	try {
		siz = 64 * 1024; //512;

		while ( 1 ) {


			if ( 10 == cnt++ ) {
				thou++;
				cnt = 0;
				std::cout << "10 ";

				if ( 1024 > siz ) {
					std::cout << siz << "B: ";
				} else {
					std::size_t tsz(siz);

					tsz /= 1024;

					if ( 1024 < tsz )
						std::cout << tsz / 1024 << "." << siz % 1024 << "MB: ";
					else 
						std::cout << siz / 1024 << "." << siz % 1024 << "KB: ";
				}
				
				ediff /= 10;
				ddiff /= 10;
				std::cout << ed.count() << "ms SET, " << dd.count() << "ms SDT, ";
				std::cout << ediff.count() << "ms AET, " << ddiff.count() << "ms ADT";
				std::cout << std::endl;

				if ( siz > SIZE_MAX / 16 ) {
					std::cout << "testing completed" << std::endl;
					return EXIT_SUCCESS;
				}

				siz *= 2;
			}

			for ( uint32_t idx = 0; idx < siz / sizeof(uint64_t); idx++) {//siz && src.size() < siz; idx++ ) {
				std::vector< uint8_t > tmp(qword_to_vector(rnd.qword()));
				src.insert(src.end(), tmp.begin(), tmp.end());
			}

			src.resize(siz);
			src_copy = src;
			
			start = std::chrono::steady_clock::now();
			try {
				ht.encode(dst, src);
			} catch ( std::exception& e ) {
				std::cerr << "ENCODING EXCEPTION: " << e.what() << std::endl;
				return EXIT_FAILURE;
			} catch ( ... ) {
				std::cerr << "UNKNOWN ENCOODING EXCEPTION" << std::endl;
				return EXIT_FAILURE;
			}

			end = std::chrono::steady_clock::now();

			if ( 1 == cnt ) {
				ed = end - start;
				ediff = end - start;
			} else
				ediff += end - start;

			dst_copy = dst;
			src.clear();
			
			start = std::chrono::steady_clock::now();
			
			try { 
				ht.decode(src, dst); 
			} catch ( std::exception& e ) { 
				std::cerr << "DECODING EXCEPTION: " << e.what() << std::endl; 
				return EXIT_FAILURE;
			}
			catch ( ... ) {
				std::cerr << "UNKNOWN DECODING EXCEPTION" << std::endl;
				return EXIT_FAILURE;
			}
			end = std::chrono::steady_clock::now();
			
			if ( 1 == cnt ) {
				dd = end - start;
				ddiff = end - start;
			}  else
				ddiff += end - start;

			if ( std::memcmp(src_copy.data(), src.data(), src_copy.size()) ) {
				std::cout << "Encode/Decode mismatch: " << std::endl;
				hex.set(src_copy);
				std::cout << "Source Copy: " << std::endl << hex.to_string() << std::endl;
				hex.set(src);
				std::cout << "Source Decode: " << std::endl << hex.to_string() << std::endl;
				hex.set(dst_copy);
				std::cout << "Compressed data: " << std::endl << hex.to_string() << std::endl;
			}
		}

	} catch ( std::exception& e ) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch ( ... ) {
		std::cerr << "Unknown exception encountered" << std::endl;
		return EXIT_FAILURE;
	}*/


	src.resize(sizeof(failed_buf_00000)); // std::strlen(reinterpret_cast<const char*>( &bytes[ 0 ] )));
	//src.resize(std::strlen(reinterpret_cast< const char* >( &bytes[ 0 ] )));
	std::memcpy(&src[ 0 ],&failed_buf_00000[ 0 ], src.size()); //  &bytes[ 0 ], src.size()); 
	//std::memcpy(&src[ 0 ], &bytes[ 0 ], src.size());

	try {
		src_copy = src;
		ht.encode(dst, src);
		dst_copy = dst;

		src.clear();
		ht.decode(src, dst);

		if ( std::memcmp(src_copy.data(), src.data(), src_copy.size()) ) {
			std::cout << "Encode/Decode mismatch: " << std::endl;
			hex.set(src_copy);
			std::cout << "Source Copy: " << std::endl << hex.to_string() << std::endl;
			hex.set(src);
			std::cout << "Source Decode: " << std::endl << hex.to_string() << std::endl;
			hex.set(dst_copy);
			std::cout << "Compressed data: " << std::endl << hex.to_string() << std::endl;
		}

	} catch ( std::exception& e ) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch ( ... ) {
		std::cerr << "unknown exception caught" << std::endl;
		return EXIT_FAILURE;
	}

	/*ht.decode(src, dst);
	hex.set(dst);
	
	std::cout << "Encoded: " << std::endl << hex.to_string() << std::endl;
	std::cout << "Decoded: " << std::endl;
	for ( auto& byte : src )
		std::cout << byte; 

	std::cout << std::endl;*/
	return EXIT_SUCCESS;
}

