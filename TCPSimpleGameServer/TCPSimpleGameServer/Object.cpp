#include "pch.h"
#include "Object.h"
#include "ProcessKeyInput.h"

Object::Object(const Position pos, SIZE boxSize, DWORD color, OBJECT_TYPE objType)
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

void Object::SetPosition(const Position pos)
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

Position Object::GetPosition() const
{
	return mPos;
}

SIZE Object::GetBoxSize() const
{
	return mBoxSize;
}

DWORD Object::GetColor() const
{
	return mColor;
}

Direction2D Object::GetDirection() const
{
	return mDirection;
}

float Object::GetVelocity() const
{
	return mVelocity;
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

bool Object::CheckCollision(Object* const other)
{
	auto [isIntersect, intersectArea] = RECTEX::Intersect(mPos, mBoxSize, other->mPos, other->mBoxSize);
	if (not isIntersect) {
		return false;
	}

	HandleCollision(other);

	return true;
}

void Object::Update(const float deltaTime)
{
	mPos.x += mDirection.dx * mVelocity * deltaTime;
	mPos.y += mDirection.dy * mVelocity * deltaTime;
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
