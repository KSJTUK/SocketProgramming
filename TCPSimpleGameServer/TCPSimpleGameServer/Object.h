#pragma once 

/* ----------------------------------------
*
*				Object
*
  ---------------------------------------- */

class Object abstract {
public:
	Object(const Position pos, SIZE boxSize, DWORD color=RGB(255, 255, 255), OBJECT_TYPE objType=NONE);
	virtual ~Object();

public:
	void SetColor(const DWORD color);
	void SetPosition(const Position pos);

	OBJECT_TYPE GetType() const;
	Position GetPosition() const;
	SIZE GetBoxSize() const;
	DWORD GetColor() const;
	Direction2D GetDirection() const;
	float GetVelocity() const;

	RECT GetBox() const;

public:
	bool CheckCollision(Object* const other);

	virtual void Update(const float deltaTime);
	virtual void HandleCollision(Object* const other) abstract;
	
private:
	Position mPos;
	SIZE mBoxSize;
	DWORD mColor;

	OBJECT_TYPE mObjType;
	Direction2D mDirection;
	float mVelocity;
};

/* ----------------------------------------
*
*				WALL
*
  ---------------------------------------- */

class Wall : public Object {
public:
	Wall(const Position pos, SIZE boxSize, DWORD color) : Object{ pos, boxSize, color, WALL } { }
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
	Bullet(const Position pos, SIZE boxSize, DWORD color) : Object{ pos, boxSize, color, BULLET } { }
	virtual ~Bullet() { }

public:
	virtual void HandleCollision(Object* other);
};