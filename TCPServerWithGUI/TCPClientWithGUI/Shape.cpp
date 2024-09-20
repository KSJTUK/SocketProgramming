#include "pch.h"
#include "Shape.h"
#include "DrawBuffer.h"

/* ----------------------------------------
*
*				Shape
*
  ---------------------------------------- */
Shape::Shape() = default;
Shape::~Shape() = default;

void Shape::SetPosition(float x, float y)
{
	mPosition = { x, y };
}

void Shape::SetPosition(Position position)
{
	mPosition = position;
}

Shape::Shape(float x, float y)
	: mPosition{ x, y }
{
}

Shape::Shape(Position position)
	: mPosition{ position }
{
}

/* ----------------------------------------
*
*				PointShape
*
  ---------------------------------------- */

PointShape::PointShape(float x, float y, std::shared_ptr<DrawBuffer> drawBuffer)
	: Shape{ x, y },
	mDrawBuffer{ drawBuffer }
{
}

PointShape::PointShape(Position position, std::shared_ptr<DrawBuffer> drawBuffer)
	: Shape{ position },
	mDrawBuffer{ drawBuffer }
{
}

void PointShape::Render()
{
	if (not mDrawBuffer->IsInCamera(GetPosition())) {
		return;
	}

	auto [x, y] = GetPosition();
	SetPixel(mDrawBuffer->GetMemDC(), (int)x, (int)y, RGB(0, 0, 0));
}

/* ----------------------------------------
*
*				Square
*
  ---------------------------------------- */

Square::Square(unsigned int width, unsigned int height, std::shared_ptr<DrawBuffer> drawBuffer)
	: Shape{ }, 
	mWidth{ width }, 
	mHeight{ height },
	mDrawBuffer{ drawBuffer } 
{ 
}

Square::Square(Position position, unsigned int width, unsigned int height, std::shared_ptr<DrawBuffer> drawBuffer)
	: Shape{ position },
	mWidth{ width },
	mHeight{ height },
	mDrawBuffer{ drawBuffer }
{
}

Square::Square(float x, float y, unsigned int width, unsigned int height, std::shared_ptr<DrawBuffer> drawBuffer)
	: Shape{ x, y },
	mWidth{ width },
	mHeight{ height },
	mDrawBuffer{ drawBuffer }
{
}

Square::~Square() = default;

void Square::Render()
{
	if (not mDrawBuffer->IsInCamera(GetPosition())) {
		return;
	}

	auto [x, y] = GetPosition();
	Rectangle(
		mDrawBuffer->GetMemDC(),
		(int)x - (mWidth / 2),   /* left   */
		(int)y - (mHeight / 2),  /* top    */
		(int)x + (mWidth / 2),   /* right  */
		(int)y + (mHeight / 2)   /* bottom */
	);
}

