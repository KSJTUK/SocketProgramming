#pragma once 

/* ----------------------------------------
*
*				Object
*
  ---------------------------------------- */

enum OBJECT_TYPE {

};

class Object {
public:
	Object(const Position pos, SIZE boxSize);
	~Object();

public:
	void SetPosition(const Position pos);
	Position GetPosition() const;

	RECT GetBox() const;

public:
	bool CheckCollision(Object* other);

	virtual void HandleCollision(Object* other);
	
private:
	Position mPos;
	SIZE mBoxSize;

	OBJECT_TYPE mObjType;
	Position mDirection;
	float mVelocity;
};