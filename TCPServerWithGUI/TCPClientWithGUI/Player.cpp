#include "pch.h"
#include "Player.h"
#include "Shape.h"
#include "GameFramework.h"
#include "Input.h"
#include "DrawBuffer.h"

Player::Player()
	: mPosition{ Random::GetUniformRandom(10.f, 1000.f), Random::GetUniformRandom(10.f, 800.f) },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer()) },
	mVelocity{ 0.3f }
{
}

Player::Player(bool playable)
	: mPosition{ Random::GetUniformRandom(10.f, 1000.f), Random::GetUniformRandom(10.f, 800.f) },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer()) },
	mVelocity{ 0.3f },
	mPlayable{ playable }
{
}

Player::Player(float x, float y)
	: mPosition{ x, y },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer()) },
	mVelocity{ 0.3f }
{
}

Player::Player(float x, float y, bool playable)
	: mPosition{ x, y },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer()) },
	mVelocity{ 0.3f },
	mPlayable{ playable }
{
}

Player::~Player() = default;

void Player::SetPosition(float x, float y)
{
	mPosition = { x, y };
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

void Player::Move()
{
	mPosition.x += mDirection.x * mVelocity;
	mPosition.y += mDirection.y * mVelocity;
}

void Player::MoveLeft()
{
	mDirection.x += DIRECTIONS[LEFT].x;
	mDirection.y += DIRECTIONS[LEFT].y;
}

void Player::MoveUp()
{
	mDirection.x += DIRECTIONS[UP].x;
	mDirection.y += DIRECTIONS[UP].y;
}

void Player::MoveRight()
{
	mDirection.x += DIRECTIONS[RIGHT].x;
	mDirection.y += DIRECTIONS[RIGHT].y;
}

void Player::MoveDown()
{
	mDirection.x += DIRECTIONS[DOWN].x;
	mDirection.y += DIRECTIONS[DOWN].y;
}

void Player::Update()
{
	std::shared_ptr<KeyInput> input = gGameFramework.GetKeyInput();
	if (mPlayable) {
		if ((*input)[VK_LEFT].down) {
			mDirection.x -= 1;
		}

		if ((*input)[VK_LEFT].up) {
			mDirection.x += 1;
		}

		if ((*input)[VK_RIGHT].down) {
			mDirection.x += 1;
		}

		if ((*input)[VK_RIGHT].up) {
			mDirection.x -= 1;
		}

		if ((*input)[VK_UP].down) {
			mDirection.y -= 1;
		}

		if ((*input)[VK_UP].up) {
			mDirection.y += 1;
		}

		if ((*input)[VK_DOWN].down) {
			mDirection.y += 1;
		}

		if ((*input)[VK_DOWN].up) {
			mDirection.y -= 1;
		}
	}

	Move();

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
