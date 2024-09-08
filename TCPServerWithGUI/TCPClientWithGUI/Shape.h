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
	Shape(int x, int y);
	Shape(Position position);
	virtual ~Shape();

public:
	Position GetPosition() const { return mPosition; }
	virtual void Render() abstract;

private:
	Position mPosition{ 0, 0 };
};


/* ----------------------------------------
*
*				PointShape
*
  ---------------------------------------- */

// PointShape 클래스
class PointShape : public Shape {
public:
	explicit PointShape(int x, int y, std::shared_ptr<DrawBuffer> drawBuffer);
	explicit PointShape(Position position, std::shared_ptr<DrawBuffer> drawBuffer);

public:
	virtual void Render() override;

private:
	std::shared_ptr<DrawBuffer> mDrawBuffer;
};


/* ----------------------------------------
*
*				Square
*
  ---------------------------------------- */

// Square 클래스
class Square : public Shape {
public:
	explicit Square(unsigned int width, unsigned int height, std::shared_ptr<DrawBuffer> drawBuffer);
	explicit Square(Position position, unsigned int width, unsigned int height, std::shared_ptr<DrawBuffer> drawBuffer);
	explicit Square(int x, int y, unsigned int width, unsigned int height, std::shared_ptr<DrawBuffer> drawBuffer);
	virtual ~Square();

public:
	virtual void Render() override;

private:
	unsigned int mWidth{ };
	unsigned int mHeight{ };
	std::shared_ptr<DrawBuffer> mDrawBuffer;
};