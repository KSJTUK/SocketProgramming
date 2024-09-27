#pragma once

#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <array>
#include <list>
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>

// concurrency support lib
#include <thread>
#include <mutex>
#include <atomic>

// Commons
#include "Macros.h"
#include "Random.h"
#include "Protocol.h"
#include "Endian.h"

inline constexpr byte NULL_CLIENT_ID = 0xFF;
inline constexpr byte MAX_CLIENT = 64;

#define NETWORK_DEBUG 1
#if NETWORK_DEBUG
inline std::mutex gIOLock;

inline std::string gPacketTypeStrs[PACKET_TYPE_COUNT]{
	"PACKET_POSITION2D",
	"PACKET_PING",
	"PACKET_PLAYER_CONNECT",
	"PACKET_PLAYER_JOIN",
	"PACKET_PLAYER_EXIT",
};
#endif

#include "TCPServerCore.h"

extern TCPServerCore gServerCore;