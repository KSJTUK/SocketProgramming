#include "pch.h"
#include "Player.h"
#include "Shape.h"
#include "GameFramework.h"
#include "Input.h"

Player::Player()
	: mPosition{ Random::GetUniformRandom(10.f, 1000.f), Random::GetUniformRandom(10.f, 800.f) },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer())}
{
}

Player::Player(float x, float y)
	: mPosition{ x, y },
	mShape{ std::make_unique<Square>(mPosition, DEFAUT_SIZE, DEFAUT_SIZE, gGameFramework.GetDrawBuffer()) }
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

void Player::Update()
{
	std::shared_ptr<KeyInput> input = gGameFramework.GetKeyInput();

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

	mVelocity = 0.01f;
	
	Move();

	mShape->SetPosition(mPosition);
}

void Player::Render()
{
	mShape->Render();
}
