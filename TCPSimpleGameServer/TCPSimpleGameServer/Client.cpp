#include "pch.h"
#include "Client.h"
#include "Object.h"
#include "Collider.h"
#include "Ray.h"

/* ----------------------------------------
*
*				Client
*
  ---------------------------------------- */

inline constexpr float MIN_WORLD_BORDER = 100.0f;
inline constexpr float DEFAULT_SPEED = 300.0f;

Client::Client()
	: mId{ NULL_CLIENT_ID },
	mCollider{ std::make_unique<Collider>() },
	mClientState{ CLIENT_STATE::EXITED },
	mMoveDirIndex{ MOVE_NONE }
{
	mInputProcessor.RegisterKeyFn(VK_LEFT, [=](float deltaTime) { MoveLeft(deltaTime); });
	mInputProcessor.RegisterKeyFn(VK_RIGHT, [=](float deltaTime) { MoveRight(deltaTime); });
	mInputProcessor.RegisterKeyFn(VK_UP, [=](float deltaTime) { MoveUp(deltaTime); });
	mInputProcessor.RegisterKeyFn(VK_DOWN, [=](float deltaTime) { MoveDown(deltaTime); });

	mCollider->SetPosition(Vec2D{ Random::GetUniformRandFloat(MIN_WORLD_BORDER, MIN_WORLD_BORDER * 2), Random::GetUniformRandFloat(MIN_WORLD_BORDER, MIN_WORLD_BORDER * 2) });
	mCollider->SetSize(SizeF{ 40.0f, 40.0f });
	mCollider->SetVelocity(DEFAULT_SPEED);
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

Vec2D Client::GetPosition() const
{
	return mCollider->GetPosition();
}

void Client::SetPosition(const Vec2D pos)
{
	mCollider->SetPosition(pos);
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
	mMoveDirIndex |= MOVE_LEFT;
}

void Client::MoveUp(float deltaTime)
{
	mMoveDirIndex |= MOVE_TOP;
}

void Client::MoveRight(float deltaTime)
{
	mMoveDirIndex |= MOVE_RIGHT;
}

void Client::MoveDown(float deltaTime)
{
	mMoveDirIndex |= MOVE_BOTTOM;
}

void Client::Update(const float deltaTime)
{
	mInputProcessor.Update(deltaTime);
	mCollider->SetDirection(directions[mMoveDirIndex]);
	mCollider->Update(deltaTime);

	mMoveDirIndex = MOVE_NONE;
}

bool Client::CheckCollision(Object* const obj)
{
	return mCollider->Intersect(*obj->GetCollider());
}

void Client::HandleCollision(Object* const obj)
{
	Vec2D slidingVector;
	SlidingVector(slidingVector, *mCollider, *(obj->GetCollider()));

	mCollider->SetPosition(mCollider->GetOldPosition() + slidingVector);
}
