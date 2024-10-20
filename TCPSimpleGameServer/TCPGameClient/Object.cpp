#include "pch.h"
#include "Object.h"

Object::Object(const Vec2D pos, SizeF boxSize, DWORD color, OBJECT_TYPE objType)
	: mPos{ pos },
	mBoxSize{ boxSize },
	mColor{ color },
	mObjType{ objType }
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

Wall::Wall(const Vec2D pos, SizeF boxSize, DWORD color)
	: Object{ pos, boxSize, color, WALL}
{ 
	SetShape(Shapes::gSquare);
}

void Wall::HandleCollision(Object* other)
{

}

/* ----------------------------------------
*
*				Bullet
*
  ---------------------------------------- */

Bullet::Bullet(const Vec2D pos, SizeF boxSize, DWORD color)
	: Object{ pos, boxSize, color, BULLET }
{
	SetShape(Shapes::gSquare);
}

void Bullet::HandleCollision(Object* other)
{

}