#pragma once

// // SDKDDKVer.h�� �����ϸ� �ְ� ������ ���뼺�� ���� Windows �÷����� ���ǵ˴ϴ�.
// ���� Windows �÷����� ���ø����̼��� �����Ϸ��� ��쿡�� SDKDDKVer.h�� �����ϱ� ����
// WinSDKVer.h�� �����ϰ� _WIN32_WINNT�� �����Ϸ��� �÷������� �����մϴ�.
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define NOMINMAX

// WinSock ��� ����
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Windows ��� ����
#include <windows.h>

#include <cstdlib>
#include <string>
#include <memory>
#include <algorithm>

#include <fstream>
#include <list>
#include <unordered_map>
#include <vector>
#include <array>
#include <functional>

#include <thread>
#include <mutex>
#include <atomic>

#include "Protocol.h"
#include "Macros.h"
#include "Random.h"

#define NETWORK_DEBUG 1

using byte = unsigned char;
using namespace std::literals;

inline constexpr byte NULL_CLIENT_ID = 0xFF;
inline constexpr SIZE WORLD_SIZE = { 5000, 5000 };

struct WindowInfo {
	HWND windowHandle;
	RECT windowRect;
};

struct Position {
	float x;
	float y;
};

struct Direction2D {
	char x;
	char y;
};

extern class GameFramework gGameFramework;