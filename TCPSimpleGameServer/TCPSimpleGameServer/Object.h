#pragma once 

/* ----------------------------------------
*
*				Object
*
  ---------------------------------------- */

class Object abstract {
public:
	Object(const Vec2D pos, SizeF boxSize, DWORD color=RGB(255, 255, 255), OBJECT_TYPE objType=NONE);
	virtual ~Object();

public:
	void SetColor(const DWORD color);
	void SetPosition(const Vec2D pos);

	OBJECT_TYPE GetType() const;
	Vec2D GetPosition() const;
	SizeF GetBoxSize() const;
	DWORD GetColor() const;
	Vec2D GetDirection() const;
	float GetVelocity() const;

	RECT GetBox() const;

public:
	bool CheckCollision(Object* const other);

	virtual void Update(const float deltaTime);
	virtual void HandleCollision(Object* const other) abstract;
	
private:
	Vec2D mPos;
	SizeF mBoxSize;
	DWORD mColor;

	OBJECT_TYPE mObjType;
	Vec2D mDirection;
	float mVelocity;
};

/* ----------------------------------------
*
*				WALL
*
  ---------------------------------------- */

class Wall : public Object {
public:
	Wall(const Vec2D pos, SizeF boxSize, DWORD color) : Object{ pos, boxSize, color, WALL } { }
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
	Bullet(const Vec2D pos, SizeF boxSize, DWORD color) : Object{ pos, boxSize, color, BULLET } { }
	virtual ~Bullet() { }

public:
	virtual void HandleCollision(Object* other);
};