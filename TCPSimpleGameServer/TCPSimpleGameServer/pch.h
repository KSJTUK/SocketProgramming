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

#include "Protocol.h"
#include "Macros.h"

inline constexpr byte MAX_CLIENT = 64;
inline constexpr byte NULL_CLIENT_ID = 0xff;

#include "ServerCore.h"

extern ServerCore gServerCore;