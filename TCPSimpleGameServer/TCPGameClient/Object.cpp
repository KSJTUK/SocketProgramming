#include "pch.h"
#include "Object.h"

Object::Object(const Position pos, SIZE boxSize, DWORD color, OBJECT_TYPE objType)
	: mPos{ pos },
	mBoxSize{ boxSize },
	mColor{ color },
	mObjType{ objType }
{
}

Object::~Object()
{
}

void Object::SetPosition(Position pos)
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
	auto [isIntersect, intersectArea] = RECTEX::Intersect(mPos, mBoxSize, other->mPos, other->mBoxSize);
	if (not isIntersect) {
		return false;
	}

	HandleCollision(other);

	return true;
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

Wall::Wall(const Position pos, SIZE boxSize, DWORD color) 
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

Bullet::Bullet(const Position pos, SIZE boxSize, DWORD color) 
	: Object{ pos, boxSize, color, BULLET }
{
	SetShape(Shapes::gSquare);
}

void Bullet::HandleCollision(Object* other)
{

}