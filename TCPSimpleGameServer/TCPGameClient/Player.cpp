#include "pch.h"
#include "Player.h"
#include "Shape.h"
#include "GameFramework.h"
#include "ServerService.h"
#include "Input.h"
#include "DrawBuffer.h"

Player::Player()
	: mPosition{ Random::GetUniformRandom(10.f, 1000.f), Random::GetUniformRandom(10.f, 800.f) },
	mSize{ DEFAULT_SIZE, DEFAULT_SIZE },
	mShape{ Shapes::gSquare },
	mVelocity{ DEFAULT_SPEED }
{
}

Player::Player(float x, float y)
	: mPosition{ x, y },
	mSize{ DEFAULT_SIZE, DEFAULT_SIZE },
	mShape{ Shapes::gSquare },
	mVelocity{ DEFAULT_SPEED }
{
}

Player::Player(const Vec2D pos)
	: mPosition{ pos },
	mSize{ DEFAULT_SIZE, DEFAULT_SIZE },
	mShape{ Shapes::gSquare },
	mVelocity{ DEFAULT_SPEED }
{
}

Player::~Player()
{
}

void Player::SetPosition(const Vec2D& position)
{
	mPosition = position;
}

void Player::SetDirection(float x, float y)
{
	mDirection = { x, y };
}

void Player::SetDirection(Vec2D dir)
{
	mDirection = dir;
}

void Player::SetValocity(float velocity)
{
	mVelocity = velocity;
}

void Player::Update(const float deltaTime)
{
}

void Player::Render()
{
	HDC memDC = gGameFramework.GetDrawBuffer()->GetMemDC();
	auto [x, y] = mPosition;
	std::string positionStr = mName + "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	int strSize = static_cast<int>(positionStr.size());
	::TextOutA(memDC, (int)x - 20 - strSize * 2, (int)y - 40, positionStr.c_str(), strSize);

	mShape->Render(mSize, mPosition, mColor, gGameFramework.GetDrawBuffer());
}