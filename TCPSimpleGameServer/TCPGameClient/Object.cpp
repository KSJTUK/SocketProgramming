#include "pch.h"
#include "Object.h"

void Object::SetShape(std::shared_ptr<class Shape> shape)
{
	mShape = shape;
}

void Object::Render(const std::shared_ptr<class DrawBuffer>& drawBuffer)
{
	mShape->Render(mBoxSize, mPos, drawBuffer);
}