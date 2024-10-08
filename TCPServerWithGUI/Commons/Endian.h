#pragma once

/*----------------------------------
*			Endian.h
* 
* 바이트 정렬방식을 통일하기 위한 헤더
*
----------------------------------*/

#include <bit>

inline constexpr std::endian APP_BYTE_ORDER = std::endian::big;
inline constexpr std::endian NATIVE_BYTE_ORDER = std::endian::native;
