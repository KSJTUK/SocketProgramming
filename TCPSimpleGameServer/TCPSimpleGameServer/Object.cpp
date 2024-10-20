#include "pch.h"
#include "Object.h"
#include "ProcessKeyInput.h"

Object::Object(const Vec2D pos, SizeF boxSize, DWORD color, OBJECT_TYPE objType)
	: mPos{ pos },
	mBoxSize{ boxSize },
	mObjType{ objType },
	mColor{ color },
	mDirection{ 0, 0 },
	mVelocity{ 0.f }
{
}

Object::~Object()
{
}

void Object::SetPosition(const Vec2D pos)
{
	mPos = pos;
}

void Object::SetColor(const DWORD color)
{
	mColor = color;
}

OBJECT_TYPE Object::GetType() const
{
	return mObjType;
}

Vec2D Object::GetPosition() const
{
	return mPos;
}

SizeF Object::GetBoxSize() const
{
	return mBoxSize;
}

DWORD Object::GetColor() const
{
	return mColor;
}

Vec2D Object::GetDirection() const
{
	return mDirection;
}

float Object::GetVelocity() const
{
	return mVelocity;
}

RECT Object::GetBox() const
{
	SizeF halfSize = { mBoxSize.w / 2, mBoxSize.h / 2 };
	int l = static_cast<int>(mPos.x - halfSize.w);
	int r = static_cast<int>(mPos.x + halfSize.w);
	int t = static_cast<int>(mPos.y - halfSize.h);
	int b = static_cast<int>(mPos.y + halfSize.h);

	return RECT{ l, t, r, b };
}

bool Object::CheckCollision(Object* const other)
{
	return true;
}

void Object::Update(const float deltaTime)
{
	mPos.x += mDirection.x * mVelocity * deltaTime;
	mPos.y += mDirection.y * mVelocity * deltaTime;
}

/* ----------------------------------------
*
*				WALL
*
  ---------------------------------------- */

void Wall::HandleCollision(Object* other)
{
}

/* ----------------------------------------
*
*				Bullet
*
  ---------------------------------------- */

void Bullet::HandleCollision(Object* other)
{
}
