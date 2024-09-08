#pragma once

/*----------------------------------
*			Protocol.h
*	Ŭ��-������ ������ ������ �ϰ�ȭ
*
----------------------------------*/

inline constexpr const char* SERVER_IP = "127.0.0.1";
inline constexpr unsigned short SERVER_PORT = 7777;

using byte = unsigned char;

#pragma pack(push, 1)
struct PacketBase {
	byte size;
	byte type;
};

struct PacketPosition2D : public PacketBase {
	int x;
	int y;
};
#pragma pack(pop)
