#pragma once

/*----------------------------------
*			Endian.h
* 
* 바이트 정렬방식을 통일하기 위한 헤더
*
----------------------------------*/


// C++ 20표준을 사용하지 않는 경우

//class Endian {
//private:
//	enum ByteOrder : char {
//		BIG,
//		LITTLE,
//		NONE
//	};
//
//	inline static ByteOrder APPLICATION_BYTE_ORDER = Endian::LITTLE;
//	inline static ByteOrder NATIVE_BYTE_ORDER = Endian::NONE;
//
//public:
//	inline static void SetNativeEndian()
//	{
//		int data = 0xff;
//		byte* byteOrder = reinterpret_cast<byte*>(&data);
//
//		if (byteOrder[0] == data) {
//			NATIVE_BYTE_ORDER = Endian::LITTLE;
//		}
//		else {
//			NATIVE_BYTE_ORDER = Endian::BIG;
//		}
//	}
//
//	inline static void AlignNativeByteOrder(char* data, size_t dataSize)
//	{
//		// 합의한 바이트 정렬방식과 시스템의 바이트 정렬 방식이 같다면 함수에선 아무일도 할 필요가 없다.
//		// 또, 데이터의 크기가 1바이트인 경우에는 바이트 정렬방식을 고려할 이유가 없다.
//		if (APPLICATION_BYTE_ORDER == NATIVE_BYTE_ORDER or dataSize == 1) {
//			return;
//		}
//
//		size_t end = dataSize - 1;
//		for (int index = 0; index < dataSize / 2; ++index) {
//			// 데이터 Swap
//			char temp = data[index];
//			data[index] = data[index + end];
//			data[index + end] = temp;
//		}
//	}
//};

// C++ 20표준을 사용하는 경우

#include <bit>
#include <algorithm>

inline constexpr std::endian APP_BYTE_ORDER = std::endian::big;
inline constexpr std::endian NATIVE_BYTE_ORDER = std::endian::native;

inline void AlignHostToApp(char* data, size_t dataSize)
{
	// 합의한 바이트 정렬방식과 시스템의 바이트 정렬 방식이 같다면 함수에선 아무일도 할 필요가 없다.
	if constexpr (APP_BYTE_ORDER != NATIVE_BYTE_ORDER) {
		// 데이터의 크기가 1바이트인 경우에는 바이트 정렬방식을 고려할 이유가 없다.
		if (dataSize == 1) {
			return;
		}

		std::reverse(data, data + dataSize);
	}
}

inline void AlignAppToHost(char* data, size_t dataSize)
{
	// 합의한 바이트 정렬방식과 시스템의 바이트 정렬 방식이 같다면 함수에선 아무일도 할 필요가 없다.
	if constexpr (APP_BYTE_ORDER != NATIVE_BYTE_ORDER) {
		// 데이터의 크기가 1바이트인 경우에는 바이트 정렬방식을 고려할 이유가 없다.
		if (dataSize == 1) {
			return;
		}

		std::reverse(data, data + dataSize);
	}
}

template <typename T>
inline void AlignHostToApp(T* data)
{
	// 합의한 바이트 정렬방식과 시스템의 바이트 정렬 방식이 같다면 함수에선 아무일도 할 필요가 없다.
	if constexpr (APP_BYTE_ORDER != NATIVE_BYTE_ORDER) {
		// 데이터의 크기가 1바이트인 경우에는 바이트 정렬방식을 고려할 이유가 없다.
		size_t dataSize = sizeof(T);
		if (dataSize == 1) {
			return;
		}

		char* bytePointer = reinterpret_cast<char*>(data);
		std::reverse(bytePointer, bytePointer + dataSize);
	}
}