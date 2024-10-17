#pragma once

//도형 클래스
class DrawBuffer;

/* ----------------------------------------
*
*				Shape
*
  ---------------------------------------- */

class Shape abstract {
public:
	Shape();
	virtual ~Shape();

public:
	virtual void Render(SIZE size, Position pos, DWORD color, const std::shared_ptr<DrawBuffer>& drawBuffer) abstract;
};


/* ----------------------------------------
*
*				PointShape
*
  ---------------------------------------- */

  // PointShape 클래스
class PointShape : public Shape {
public:
	explicit PointShape();

public:
	virtual void Render(SIZE size, Position pos, DWORD color, const std::shared_ptr<DrawBuffer>& drawBuffer) override;
};


/* ----------------------------------------
*
*				Square
*
  ---------------------------------------- */

  // Square 클래스
class Square : public Shape {
public:
	explicit Square();
	virtual ~Square();

public:
	virtual void Render(SIZE size, Position pos, DWORD color, const std::shared_ptr<DrawBuffer>& drawBuffer) override;
};

/* ----------------------------------------
*
*				Circle
*
  ---------------------------------------- */

class Circle : public Shape {
public:
	explicit Circle();
	virtual ~Circle();

public:
	virtual void Render(SIZE size, Position pos, DWORD color, const std::shared_ptr<DrawBuffer>& drawBuffer) override;
};

/* ----------------------------------------
*
*				eclipse
*
  ---------------------------------------- */