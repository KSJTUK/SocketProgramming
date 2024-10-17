#pragma once 

/* ----------------------------------------
*
*				Object
*
  ---------------------------------------- */

class Object abstract {
public:
	Object(const Position pos, SIZE boxSize, DWORD color=RGB(255, 255, 255), OBJECT_TYPE objType = NONE);
	virtual ~Object();

public:
	void SetShape(std::shared_ptr<class Shape> shape);
	void SetPosition(const Position pos);
	void SetColor(DWORD color);

public:
	bool CheckCollision(Object* const other);
	virtual void HandleCollision(Object* const other) abstract;
	virtual void Render(const std::shared_ptr<class DrawBuffer>& drawBuffer);

private:
	Position mPos;
	SIZE mBoxSize;

	OBJECT_TYPE mObjType;
	Direction2D mDirection;
	float mVelocity;

	DWORD mColor{ };

	std::shared_ptr<class Shape> mShape;
};

/* ----------------------------------------
*
*				WALL
*
  ---------------------------------------- */

class Wall : public Object {
public:
	Wall(const Position pos, SIZE boxSize, DWORD color);
	virtual ~Wall() { }

public:
	virtual void HandleCollision(Object* other);
};

/* ----------------------------------------
*
*				Bullet
*
  ---------------------------------------- */

class Bullet : public Object {
public:
	Bullet(const Position pos, SIZE boxSize, DWORD color);
	virtual ~Bullet() { }

public:
	virtual void HandleCollision(Object* other);
};