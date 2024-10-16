#pragma once 

/* ----------------------------------------
*
*				Object
*
  ---------------------------------------- */

class Object abstract {
public:
	Object(const Position pos, SIZE boxSize, OBJECT_TYPE objType=NONE);
	virtual ~Object();

public:
	void SetPosition(const Position pos);
	OBJECT_TYPE GetType() const;
	Position GetPosition() const;
	SIZE GetBoxSize() const;
	Direction2D GetDirection() const;
	float GetVelocity() const;

	RECT GetBox() const;

public:
	bool CheckCollision(Object* other);

	virtual void Update(const float deltaTime);
	virtual void HandleCollision(Object* other) abstract;
	
private:
	Position mPos;
	SIZE mBoxSize;

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
	Wall(const Position pos, SIZE boxSize) : Object{ pos, boxSize, WALL } { }
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
	Bullet(const Position pos, SIZE boxSize) : Object{ pos, boxSize, BULLET } { }
	virtual ~Bullet() { }

public:
	virtual void HandleCollision(Object* other);
};