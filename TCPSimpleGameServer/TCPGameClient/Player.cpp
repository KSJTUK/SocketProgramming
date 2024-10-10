#include "pch.h"
#include "Player.h"
#include "Shape.h"
#include "GameFramework.h"
#include "ServerService.h"
#include "Input.h"
#include "DrawBuffer.h"

Player::Player()
	: mPosition{ Random::GetUniformRandom(10.f, 1000.f), Random::GetUniformRandom(10.f, 800.f) },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer()) },
	mVelocity{ DEFAULT_SPEED }
{
}

Player::Player(float x, float y)
	: mPosition{ x, y },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer()) },
	mVelocity{ DEFAULT_SPEED }
{
}

Player::Player(const Position pos)
	: mPosition{ pos },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer()) },
	mVelocity{ DEFAULT_SPEED }
{
}

Player::~Player()
{
}

void Player::SetPosition(const Position& position)
{
	mPosition = position;
}

void Player::SetDirection(char x, char y)
{
	mDirection = { x, y };
}

void Player::SetDirection(Direction2D dir)
{
	mDirection = dir;
}

void Player::SetValocity(float velocity)
{
	mVelocity = velocity;
}

void Player::RegisterExecutionFn()
{
	gGameFramework.GetKeyInput()->RegisterKeyFn(VK_LEFT, [=](float deltaTime) { gServerService.Send<PacketPlayerInput>(PACKET_PLAYER_INPUT, VK_LEFT); });
	gGameFramework.GetKeyInput()->RegisterKeyFn(VK_RIGHT, [=](float deltaTime) { gServerService.Send<PacketPlayerInput>(PACKET_PLAYER_INPUT, VK_RIGHT); });
	gGameFramework.GetKeyInput()->RegisterKeyFn(VK_UP, [=](float deltaTime) { gServerService.Send<PacketPlayerInput>(PACKET_PLAYER_INPUT, VK_UP); });
	gGameFramework.GetKeyInput()->RegisterKeyFn(VK_DOWN, [=](float deltaTime) { gServerService.Send<PacketPlayerInput>(PACKET_PLAYER_INPUT, VK_DOWN); });
}

void Player::Update(const float deltaTime)
{
	mShape->SetPosition(mPosition);
}

void Player::Render()
{
	HDC memDC = gGameFramework.GetDrawBuffer()->GetMemDC();
	auto [x, y] = mPosition;
	std::string positionStr = mName + "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	int strSize = static_cast<int>(positionStr.size());
	::TextOutA(memDC, (int)x - 20 - strSize * 2, (int)y - 40, positionStr.c_str(), strSize);

	mShape->Render();
}