#pragma once

/*----------------------------------
*			Protocol.h
*	Ŭ��-������ ������ ������ �ϰ�ȭ
*
----------------------------------*/

#include <chrono>
#include "Types.h"

#define USE_LOOP_BACK 1

// �ζ��� ��� ����
#if USE_LOOP_BACK
inline constexpr const char* SERVER_IP = "127.0.0.1";
#else
inline constexpr const char* SERVER_IP = "192.168.23.192";
#endif
inline constexpr unsigned short SERVER_PORT = 7777;
inline constexpr unsigned short MAX_OBJECT = 1000;

using byte = unsigned char;

enum OBJECT_TYPE {
	NONE,
	WALL,
	BULLET
};

enum {
	PACKET_POSITION2D = 0,
	PACKET_PING,
	PACKET_PLAYER_INPUT,
	PACKET_PLAYER_CONNECT,
	PACKET_PLAYER_JOIN,
	PACKET_PLAYER_EXIT,
	PACKET_OBJECT_INFO,

	PACKET_TYPE_COUNT,
};

struct ObjectInfo {
	Vec2D pos;
	SIZE boxSize;
	OBJECT_TYPE objectType;

	Vec2D dir;
	float velocity;
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

struct PacketPlayerInput : public PacketBase {
	byte keyInput;
	bool down;
};

struct PacketPlayerJoin : public PacketBase {
	Vec2D pos;
};

struct PacketPlayerExit : public PacketBase {
};

struct PacketPosition2D : public PacketBase {
	Vec2D pos;
};

struct PacketObjectInfo : public PacketBase {
	Vec2D pos;
	SizeF boxSize;
	OBJECT_TYPE objectType;
	DWORD color;

	Vec2D dir;
	float velocity;
	unsigned short objectIndex;
};

struct PacketFireBullet : public PacketBase {
	Vec2D pos;
	SIZE boxSize;
	OBJECT_TYPE objectType;
	DWORD color;

	Vec2D dir;
	float velocity;
	unsigned short objectIndex;
};

#pragma pack(pop)