#include "pch.h"
#include "Client.h"

/* ----------------------------------------
*
*				Client
*
  ---------------------------------------- */

inline constexpr float MIN_WORLD_BORDER = 100.0f;
inline constexpr float DEFAULT_SPEED = 30.0f;

Client::Client()
	: mId{ NULL_CLIENT_ID },
	//mPosition{ Random::GetUniformRandFloat(MIN_WORLD_BORDER, WORLD_SIZE.cx - MIN_WORLD_BORDER), Random::GetUniformRandFloat(MIN_WORLD_BORDER, WORLD_SIZE.cy - MIN_WORLD_BORDER) },
	mPosition{ Random::GetUniformRandFloat(MIN_WORLD_BORDER, MIN_WORLD_BORDER * 2), Random::GetUniformRandFloat(MIN_WORLD_BORDER, MIN_WORLD_BORDER * 2) },
	mVelocity{ DEFAULT_SPEED },
	mClientState{ CLIENT_STATE::EXITED }
{
	mInputProcessor.RegisterKeyFn(VK_LEFT, [=](float deltaTime) { MoveLeft(deltaTime); });
	mInputProcessor.RegisterKeyFn(VK_RIGHT, [=](float deltaTime) { MoveRight(deltaTime); });
	mInputProcessor.RegisterKeyFn(VK_UP, [=](float deltaTime) { MoveUp(deltaTime); });
	mInputProcessor.RegisterKeyFn(VK_DOWN, [=](float deltaTime) { MoveDown(deltaTime); });
}

Client::~Client()
{
}

Transceiver& Client::GetTransceiver()
{
	return mTransceiver;
}

ProcessKeyInput& Client::GetInputProcessor()
{
	return mInputProcessor;
}

const byte Client::GetId() const
{
	return mId;
}

Position Client::GetPosition() const
{
	return mPosition;
}

void Client::SetPosition(const Position pos)
{
	mPosition = pos;
}

void Client::Join(SOCKET clientSocket, byte id)
{
	mTransceiver.Init(clientSocket);
	mId = id;
	mClientState = CLIENT_STATE::JOINED;
}

void Client::Exit()
{
	mId = NULL_CLIENT_ID;
	mClientState = CLIENT_STATE::EXITED;
}

void Client::MoveLeft(float deltaTime)
{
	mPosition.x -= mVelocity * deltaTime;
}

void Client::MoveUp(float deltaTime)
{
	mPosition.y -= mVelocity * deltaTime;
}

void Client::MoveRight(float deltaTime)
{
	mPosition.x += mVelocity * deltaTime;
}

void Client::MoveDown(float deltaTime)
{
	mPosition.y += mVelocity * deltaTime;
}

void Client::Update(const float deltaTime)
{
	mInputProcessor.Update(deltaTime);
}
