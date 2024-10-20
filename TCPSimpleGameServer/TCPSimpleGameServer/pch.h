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

enum MOVE_DIR : short {
	MOVE_NONE = 0x00,
	MOVE_LEFT = 0x01,
	MOVE_TOP = 0x02,
	MOVE_RIGHT = 0x04,
	MOVE_BOTTOM = 0x08,
	MOVE_DIR_COUNT = (MOVE_BOTTOM | MOVE_RIGHT | MOVE_TOP | MOVE_LEFT) + 1
};

inline Vec2D directions[MOVE_DIR_COUNT]{
	Vec2D{ 0.0f, 0.0f },					// NONE
	Vec2D::GetNormalized({ -1.0f, 0.0f }),	// LEFT
	Vec2D::GetNormalized({ 0.0f, -1.0f }),	// TOP
	Vec2D::GetNormalized({ -1.0f, -1.0f }),	// LEFT | TOP
	Vec2D::GetNormalized({1.0f, 0.0f}),		// RIGHT
	Vec2D{ 0.0f, 0.0f },					// RIGHT | LEFT
	Vec2D::GetNormalized({ 1.0f, -1.0f }),	// RIGHT | TOP
	Vec2D::GetNormalized({ 0.0f, -1.0f }),	// RIGHT | TOP | LEFT
	Vec2D::GetNormalized({ 0.0f, 1.0f }),	// BOTTOM
	Vec2D::GetNormalized({ -1.0f, 1.0f }),	// BOTTOM | LEFT
	Vec2D{ 0.0f, 0.0f },					// BOTTOM | TOP
	Vec2D::GetNormalized({ -1.0f, 0.0f }),	// BOTTOM | TOP | LEFT
	Vec2D::GetNormalized({ 1.0f, 1.0f }),	// BOTTOM | RIGHT
	Vec2D::GetNormalized({ 0.0f, 1.0f }),	// BOTTOM | RIGHT | LEFT
	Vec2D::GetNormalized({ 1.0f, 0.0f }),	// BOTTOM | RIGHT | TOP
	Vec2D{ 0.0f, 0.0f },					// BOTTOM | RIGHT | TOP | LEFT
};

#include "GameServer.h"

extern GameServer gGameServer;