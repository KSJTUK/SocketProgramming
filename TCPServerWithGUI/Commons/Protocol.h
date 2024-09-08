#pragma once

/*----------------------------------
*			Protocol.h
*	Ŭ��-������ ������ ������ �ϰ�ȭ
*
----------------------------------*/

inline constexpr const char* SERVER_IP = "183.101.111.33";
inline constexpr unsigned short SERVER_PORT = 7777;

using byte = unsigned char;

enum {
	PACKET_POSITION2D = 0,
};

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
