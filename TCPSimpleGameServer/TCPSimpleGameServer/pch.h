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

inline constexpr byte MAX_CLIENT = 64;
inline constexpr byte NULL_CLIENT_ID = 0xff;

inline constexpr SIZE worldSize = { 5000, 5000 };
inline constexpr SIZE worldCellSize = { 100, 100 };

#include "ServerCore.h"

extern ServerCore gServerCore;