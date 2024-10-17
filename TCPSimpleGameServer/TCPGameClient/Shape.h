#pragma once

//���� Ŭ����
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

  // PointShape Ŭ����
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

  // Square Ŭ����
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