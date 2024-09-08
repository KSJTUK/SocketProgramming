#pragma once

// // SDKDDKVer.h를 포함하면 최고 수준의 가용성을 가진 Windows 플랫폼이 정의됩니다.
// 이전 Windows 플랫폼용 애플리케이션을 빌드하려는 경우에는 SDKDDKVer.h를 포함하기 전에
// WinSDKVer.h를 포함하고 _WIN32_WINNT를 지원하려는 플랫폼으로 설정합니다.
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// WinSock 헤더 파일
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Windows 헤더 파일
#include <windows.h>

#include <cstdlib>
#include <string>
#include <memory>

#include <list>
#include <vector>

#include "Random.h"

struct WindowInfo {
	HWND windowHandle;
	RECT windowRect;
};

struct Position {
	int x;
	int y;
};