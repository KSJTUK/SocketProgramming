#include "pch.h"
#include "Object.h"

Object::Object()
	: mPos{ },
	mBoxSize{ },
	mColor{ },
	mObjType{ NONE },
	mVelocity{ },
	mActive{ false }
{
}

Object::Object(OBJECT_TYPE objType)
	: mPos{ },
	mBoxSize{ },
	mColor{ },
	mObjType{ objType },
	mVelocity{ },
	mActive{ false }
{
}

Object::Object(const Vec2D pos, SizeF boxSize, DWORD color, OBJECT_TYPE objType)
	: mPos{ pos },
	mBoxSize{ boxSize },
	mColor{ color },
	mObjType{ objType },
	mVelocity{ },
	mActive{ false }
{
}

Object::~Object()
{
}

void Object::SetPosition(Vec2D pos)
{
	mPos = pos;
}

void Object::SetColor(DWORD color)
{
	mColor = color;
}

void Object::SetShape(std::shared_ptr<class Shape> shape)
{
	mShape = shape;
}

void Object::SetSize(const SizeF size)
{
	mBoxSize = size;
}

bool Object::CheckCollision(Object* const other)
{
	return false;
}

void Object::Render(const std::shared_ptr<class DrawBuffer>& drawBuffer)
{
	mShape->Render(mBoxSize, mPos, mColor, drawBuffer);
}

/* ----------------------------------------
*
*				Wall
*
  ---------------------------------------- */

Wall::Wall()
	: Object{ WALL }
{
}

Wall::Wall(const Vec2D pos, SizeF boxSize, DWORD color)
	: Object{ pos, boxSize, color, WALL}
{ 
}

void Wall::HandleCollision(Object* other)
{

}

/* ----------------------------------------
*
*				Bullet
*
  ---------------------------------------- */

Bullet::Bullet()
	: Object{ BULLET }
{
}

Bullet::Bullet(const Vec2D pos, SizeF boxSize, DWORD color)
	: Object{ pos, boxSize, color, BULLET }
{
}

void Bullet::HandleCollision(Object* other)
{

}

/* ----------------------------------------
*
*				Ball
*
  ---------------------------------------- */

Ball::Ball()
	: Object{ BALL }
{
}

Ball::Ball(const Vec2D pos, SizeF boxSize, DWORD color)
	: Object{ pos, boxSize, color, BALL}
{
}

void Ball::HandleCollision(Object* other)
{

}