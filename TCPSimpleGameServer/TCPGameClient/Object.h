#pragma once 

/* ----------------------------------------
*
*				Object
*
  ---------------------------------------- */

class Object abstract {
public:
	Object();
	Object(OBJECT_TYPE objType);
	Object(const Vec2D pos, SizeF boxSize, DWORD color=RGB(255, 255, 255), OBJECT_TYPE objType = NONE);
	virtual ~Object();

public:
	void SetShape(std::shared_ptr<class Shape> shape);
	void SetPosition(const Vec2D pos);
	void SetColor(DWORD color);
	void SetSize(SizeF size);

public:
	bool CheckCollision(Object* const other);
	virtual void HandleCollision(Object* const other) abstract;
	virtual void Render(const std::shared_ptr<class DrawBuffer>& drawBuffer);

private:
	Vec2D mPos;
	SizeF mBoxSize;

	OBJECT_TYPE mObjType;
	Vec2D mDirection;
	float mVelocity;

	DWORD mColor;

	bool mActive;

	std::shared_ptr<class Shape> mShape;
};

/* ----------------------------------------
*
*				WALL
*
  ---------------------------------------- */

class Wall : public Object {
public:
	Wall();
	Wall(const Vec2D pos, SizeF boxSize, DWORD color);
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
	Bullet();
	Bullet(const Vec2D pos, SizeF boxSize, DWORD color);
	virtual ~Bullet() { }

public:
	virtual void HandleCollision(Object* other);
};

/* ----------------------------------------
*
*				Ball
*
  ---------------------------------------- */

class Ball : public Object {
public:
	Ball();
	Ball(const Vec2D pos, SizeF boxSize, DWORD color);
	virtual ~Ball() { }

public:
	virtual void HandleCollision(Object* other);
};