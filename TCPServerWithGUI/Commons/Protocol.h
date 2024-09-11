#pragma once

/*----------------------------------
*			Protocol.h
*	Ŭ��-������ ������ ������ �ϰ�ȭ
*
----------------------------------*/

#define USE_LOOP_BACK 0

#if USE_LOOP_BACK
inline constexpr const char* SERVER_IP = "127.0.0.1";
#else
inline constexpr const char* SERVER_IP = "183.101.111.33";
#endif
inline constexpr unsigned short SERVER_PORT = 7777;

using byte = unsigned char;

enum {
	PACKET_POSITION2D = 0,
	PACKET_PING,
	PACKET_PLAYER_CONNECT,
	PACKET_PLAYER_JOIN,
	PACKET_PLAYER_EXIT,
};

#pragma pack(push, 1)
struct PacketBase {
	byte size;
	byte type;
	byte senderId;
};

struct PacketPing : public PacketBase {
	std::chrono::steady_clock::time_point timeSent;
};

struct PacketPlayerConnect : public PacketBase {
};

struct PacketPlayerJoin : public PacketBase { 
	float x;
	float y;
};

struct PacketPlayerExit : public PacketBase {
};

struct PacketPosition2D : public PacketBase {
	float x;
	float y;
};

#pragma pack(pop)
