#pragma once

#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <array>
#include <list>
#include <vector>
#include <queue>
#include <unordered_map>

#include <iostream>
#include <utility>
#include <functional>
#include <memory>
#include <algorithm>
#include <numeric>

// concurrency support lib
#include <thread>
#include <mutex>
#include <atomic>

#include "Protocol.h"
#include "Macros.h"
#include "Random.h"
#include "Collider.h"

inline constexpr byte MAX_CLIENT = 64;
inline constexpr byte NULL_CLIENT_ID = 0xff;

inline constexpr SIZE WORLD_SIZE = { 5000, 5000 };
inline constexpr SIZE WORLD_CELL_SIZE = { 100, 100 };

#include "GameServer.h"

extern GameServer gGameServer;