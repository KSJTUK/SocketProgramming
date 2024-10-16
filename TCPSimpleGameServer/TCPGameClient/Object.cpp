#include "pch.h"
#include "Object.h"

Object::Object(const Position pos, SIZE boxSize, OBJECT_TYPE objType)
	: mPos{ pos },
	mBoxSize{ boxSize },
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

void Object::Render(const std::shared_ptr<class DrawBuffer>& drawBuffer)
{
	mShape->Render(mBoxSize, mPos, drawBuffer);
}

/* ----------------------------------------
*
*				Wall
*
  ---------------------------------------- */

Wall::Wall(const Position pos, SIZE boxSize) 
	: Object{ pos, boxSize, WALL } 
{ 
	SetShape(Shapes::gSquare);
	SetColor(RGB(255, 0, 0));
}

void Wall::HandleCollision(Object* other)
{

}

/* ----------------------------------------
*
*				Bullet
*
  ---------------------------------------- */

Bullet::Bullet(const Position pos, SIZE boxSize) 
	: Object{ pos, boxSize, BULLET }
{
	SetShape(Shapes::gSquare);
}

void Bullet::HandleCollision(Object* other)
{

}