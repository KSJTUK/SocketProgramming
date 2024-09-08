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

Shape::Shape(int x, int y)
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

PointShape::PointShape(int x, int y, std::shared_ptr<DrawBuffer> drawBuffer)
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
	auto [x, y] = GetPosition();
	SetPixel(mDrawBuffer->GetMemDC(), x, y, RGB(255, 255, 255));
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

Square::Square(int x, int y, unsigned int width, unsigned int height, std::shared_ptr<DrawBuffer> drawBuffer)
	: Shape{ x, y },
	mWidth{ width },
	mHeight{ height },
	mDrawBuffer{ drawBuffer }
{
}

Square::~Square() = default;

void Square::Render()
{
	auto [x, y] = GetPosition();
	Rectangle(
		mDrawBuffer->GetMemDC(),
		x - (mWidth / 2),   /* left   */
		y - (mHeight / 2),  /* top    */
		x + (mWidth / 2),   /* right  */
		y + (mHeight / 2)   /* bottom */
	);
}

