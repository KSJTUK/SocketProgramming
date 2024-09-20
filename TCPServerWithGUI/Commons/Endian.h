#pragma once

/*----------------------------------
*			Endian.h
* 
* ����Ʈ ���Ĺ���� �����ϱ� ���� ���
*
----------------------------------*/


// C++ 20ǥ���� ������� �ʴ� ���

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
//		// ������ ����Ʈ ���Ĺ�İ� �ý����� ����Ʈ ���� ����� ���ٸ� �Լ����� �ƹ��ϵ� �� �ʿ䰡 ����.
//		// ��, �������� ũ�Ⱑ 1����Ʈ�� ��쿡�� ����Ʈ ���Ĺ���� ����� ������ ����.
//		if (APPLICATION_BYTE_ORDER == NATIVE_BYTE_ORDER or dataSize == 1) {
//			return;
//		}
//
//		size_t end = dataSize - 1;
//		for (int index = 0; index < dataSize / 2; ++index) {
//			// ������ Swap
//			char temp = data[index];
//			data[index] = data[index + end];
//			data[index + end] = temp;
//		}
//	}
//};

// C++ 20ǥ���� ����ϴ� ���

#include <bit>
#include <algorithm>

inline constexpr std::endian APP_BYTE_ORDER = std::endian::big;
inline constexpr std::endian NATIVE_BYTE_ORDER = std::endian::native;

inline void AlignHostToApp(char* data, size_t dataSize)
{
	// ������ ����Ʈ ���Ĺ�İ� �ý����� ����Ʈ ���� ����� ���ٸ� �Լ����� �ƹ��ϵ� �� �ʿ䰡 ����.
	if constexpr (APP_BYTE_ORDER != NATIVE_BYTE_ORDER) {
		// �������� ũ�Ⱑ 1����Ʈ�� ��쿡�� ����Ʈ ���Ĺ���� ����� ������ ����.
		if (dataSize == 1) {
			return;
		}

		std::reverse(data, data + dataSize);
	}
}

inline void AlignAppToHost(char* data, size_t dataSize)
{
	// ������ ����Ʈ ���Ĺ�İ� �ý����� ����Ʈ ���� ����� ���ٸ� �Լ����� �ƹ��ϵ� �� �ʿ䰡 ����.
	if constexpr (APP_BYTE_ORDER != NATIVE_BYTE_ORDER) {
		// �������� ũ�Ⱑ 1����Ʈ�� ��쿡�� ����Ʈ ���Ĺ���� ����� ������ ����.
		if (dataSize == 1) {
			return;
		}

		std::reverse(data, data + dataSize);
	}
}

template <typename T>
inline void AlignHostToApp(T* data)
{
	// ������ ����Ʈ ���Ĺ�İ� �ý����� ����Ʈ ���� ����� ���ٸ� �Լ����� �ƹ��ϵ� �� �ʿ䰡 ����.
	if constexpr (APP_BYTE_ORDER != NATIVE_BYTE_ORDER) {
		// �������� ũ�Ⱑ 1����Ʈ�� ��쿡�� ����Ʈ ���Ĺ���� ����� ������ ����.
		size_t dataSize = sizeof(T);
		if (dataSize == 1) {
			return;
		}

		char* bytePointer = reinterpret_cast<char*>(data);
		std::reverse(bytePointer, bytePointer + dataSize);
	}
}