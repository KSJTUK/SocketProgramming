#include "pch.h"
#include "Object.h"
#include "ProcessKeyInput.h"

Object::Object()
	: mCollider{ std::make_unique<Collider>() },
	mObjType{ NONE },
	mColor{ RGB(255, 255, 255) },
	mActive{ false }
{
}

Object::Object(OBJECT_TYPE objType)
	: mCollider{ std::make_unique<Collider>() },
	mObjType{ objType },
	mColor{ RGB(255, 255, 255) },
	mActive{ false }
{
}

Object::Object(const Vec2D pos, SizeF boxSize, DWORD color, OBJECT_TYPE objType)
	: mCollider{ std::make_unique<Collider>(pos, boxSize) },
	mObjType{ objType },
	mColor{ color },
	mActive{ false }
{
	mCollider->SetDirection(Vec2D{ });
	mCollider->SetVelocity(0.0f);
}

Object::~Object()
{
}

void Object::SetPosition(const Vec2D pos)
{
	mCollider->SetPosition(pos);
}

void Object::SetSize(const SizeF size)
{
	mCollider->SetSize(size);
}

void Object::SetActive(bool active)
{
	mActive = active;
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
	return mCollider->GetPosition();
}

SizeF Object::GetBoxSize() const
{
	return mCollider->GetSize();
}

DWORD Object::GetColor() const
{
	return mColor;
}

Vec2D Object::GetDirection() const
{
	return mCollider->GetDirection();
}

float Object::GetVelocity() const
{
	return mCollider->GetVelocity();
}

RECT Object::GetBox() const
{
	return mCollider->Rect();
}

const Collider* Object::GetCollider() const
{
	return mCollider.get();
}

void Object::Reset()
{
	mCollider->Reset();
}

bool Object::CheckCollision(Object* const other)
{
	return true;
}

void Object::Update(const float deltaTime)
{
	mCollider->Update(deltaTime);
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

/* ----------------------------------------
*
*				Ball
*
  ---------------------------------------- */

void Ball::HandleCollision(Object* other)
{
	
}