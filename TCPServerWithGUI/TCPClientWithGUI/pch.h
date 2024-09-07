#pragma once

// // SDKDDKVer.h�� �����ϸ� �ְ� ������ ���뼺�� ���� Windows �÷����� ���ǵ˴ϴ�.
// ���� Windows �÷����� ���ø����̼��� �����Ϸ��� ��쿡�� SDKDDKVer.h�� �����ϱ� ����
// WinSDKVer.h�� �����ϰ� _WIN32_WINNT�� �����Ϸ��� �÷������� �����մϴ�.
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// WinSock ��� ����
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Windows ��� ����
#include <windows.h>

#include <cstdlib>
#include <string>
#include <memory>

struct WindowInfo {
	HWND windowHandle;
	RECT windowRect;
};