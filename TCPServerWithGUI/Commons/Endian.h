#pragma once

/*----------------------------------
*			Endian.h
* 
* ����Ʈ ���Ĺ���� �����ϱ� ���� ���
*
----------------------------------*/

#include <bit>

inline constexpr std::endian APP_BYTE_ORDER = std::endian::big;
inline constexpr std::endian NATIVE_BYTE_ORDER = std::endian::native;
