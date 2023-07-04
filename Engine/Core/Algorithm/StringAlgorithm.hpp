//
// Created by loyus on 7/4/2023.
//

#pragma once

#include <stdint.h>

inline constexpr uint64_t
HashString(auto &&Str)
{
	// FNV-1a 64 bit algorithm
	uint64_t result = 0xcbf29ce484222325;// FNV offset basis

	for (char c: Str) {
		result ^= c;
		result *= 1099511628211;// FNV prime
	}

	return result;
}