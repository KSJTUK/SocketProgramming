#pragma once 

/* ----------------------------------------
*
*				Object
*
  ---------------------------------------- */

class Object abstract {
public:
	Object(const Position pos, SIZE boxSize, OBJECT_TYPE objType = NONE);
	virtual ~Object();

public:
	void SetShape(std::shared_ptr<class Shape> shape);
	void SetPosition(const Position pos);
	void SetColor(DWORD color);
	//Position GetPosition() const;
	//SIZE GetBoxSize() const;
	//Direction2D GetDirection() const;
	//float GetVelocity() const;

	//RECT GetBox() const;

public:
	//virtual void Update(const float deltaTime);
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
	Wall(const Position pos, SIZE boxSize);
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
	Bullet(const Position pos, SIZE boxSize);
	virtual ~Bullet() { }

public:
	virtual void HandleCollision(Object* other);
};