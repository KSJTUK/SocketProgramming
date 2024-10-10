#include "pch.h"
#include "Object.h"
#include "ProcessKeyInput.h"

Object::Object(Position pos, SIZE boxSize)
	: mPos{ pos },
	mBoxSize{ boxSize },
	mVelocity{ 0.f }
{
}

Object::~Object()
{
}

void Object::SetPosition(Position pos)
{
	mPos = pos;
}

Position Object::GetPosition() const
{
	return mPos;
}

RECT Object::GetBox() const
{
	SIZE halfSize = { mBoxSize.cx / 2, mBoxSize.cy / 2 };
	int l = static_cast<int>(mPos.x) - halfSize.cx;
	int r = static_cast<int>(mPos.x) + halfSize.cx;
	int t = static_cast<int>(mPos.y) - halfSize.cy;
	int b = static_cast<int>(mPos.y) + halfSize.cy;

	return RECT{ l, t, r, b };
}

bool Object::CheckCollision(Object* other)
{
	auto b1 = GetBox();
	auto b2 = other->GetBox();

	if ((b1.left > b2.right or b1.right < b2.left)
		or b1.top > b2.bottom or b1.bottom < b2.top) {
		return false;
	}

	HandleCollision(other);

	return true;
}

void Object::HandleCollision(Object* other)
{
	HandleCollision(other);
}

/* ----------------------------------------
*
*				Player
*
  ---------------------------------------- */

inline constexpr float WORLD_BORDER = 100.f;

Player::Player()
	: Object{ { Random::GetUniformRandom(WORLD_BORDER, worldSize.cx - WORLD_BORDER), Random::GetUniformRandom(WORLD_BORDER, worldSize.cy - WORLD_BORDER) }, { DEFAULT_SIZE, DEFAULT_SIZE } },
	mInputProcessor{ std::make_unique<ProcessKeyInput>() },
	mVelocity{ DEFAULT_SPEED }
{
	RegisterExecutionFn();
}

Player::Player(float x, float y)
	: Object{ { x, y }, { DEFAULT_SIZE, DEFAULT_SIZE } },
	mInputProcessor{ std::make_unique<ProcessKeyInput>() },
	mVelocity{ DEFAULT_SPEED }
{
	RegisterExecutionFn();
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
	mInputProcessor->RegisterKeyFn(VK_LEFT, [=](float deltaTime) { MoveLeft(deltaTime); });
	mInputProcessor->RegisterKeyFn(VK_RIGHT, [=](float deltaTime) { MoveRight(deltaTime); });
	mInputProcessor->RegisterKeyFn(VK_UP, [=](float deltaTime) { MoveUp(deltaTime); });
	mInputProcessor->RegisterKeyFn(VK_DOWN, [=](float deltaTime) { MoveDown(deltaTime); });
}

void Player::MoveLeft(float deltaTime)
{
	mPosition.x -= mVelocity * deltaTime;
}

void Player::MoveUp(float deltaTime)
{
	mPosition.y -= mVelocity * deltaTime;
}

void Player::MoveRight(float deltaTime)
{
	mPosition.x += mVelocity * deltaTime;
}

void Player::MoveDown(float deltaTime)
{
	mPosition.y += mVelocity * deltaTime;
}

void Player::Update(const float deltaTime, byte input)
{
}

void Player::HandleCollision(Object* other)
{
}
