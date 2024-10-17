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

/* ----------------------------------------
*
*				PointShape
*
  ---------------------------------------- */

PointShape::PointShape()
	: Shape{ }
{
} 

void PointShape::Render(SIZE size, Position pos, DWORD color, const std::shared_ptr<class DrawBuffer>& drawBuffer)
{
	if (not drawBuffer->IsInCamera(pos)) {
		return;
	}

	auto [x, y] = pos;
	SetPixel(drawBuffer->GetMemDC(), (int)x, (int)y, color);
}

/* ----------------------------------------
*
*				Square
*
  ---------------------------------------- */

Square::Square()
	: Shape{ }
{
}

Square::~Square() = default;

void Square::Render(SIZE size, Position pos, DWORD color, const std::shared_ptr<class DrawBuffer>& drawBuffer)
{
	if (not drawBuffer->IsInCamera(pos)) {
		return;
	}

	drawBuffer->SetBrush(color);

	auto [x, y] = pos;
	auto [w, h] = size;
	Rectangle(
		drawBuffer->GetMemDC(),
		(int)x - (w / 2),   /* left   */
		(int)y - (h / 2),  /* top    */
		(int)x + (w / 2),   /* right  */
		(int)y + (h / 2)   /* bottom */
	);

	drawBuffer->ResetBrush();
}

/* ----------------------------------------
*
*				Circle
*
  ---------------------------------------- */

Circle::Circle()
	: Shape{ }
{
}

Circle::~Circle()
{
}

void Circle::Render(SIZE size, Position pos, DWORD color, const std::shared_ptr<DrawBuffer>& drawBuffer)
{
	if (not drawBuffer->IsInCamera(pos)) {
		return;
	}

	drawBuffer->SetBrush(color);

	drawBuffer->ResetBrush();
}
