#include "pch.h"
#include "Object.h"
#include "ProcessKeyInput.h"

Object::Object(const Position pos, SIZE boxSize)
	: mPos{ pos },
	mBoxSize{ boxSize },
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