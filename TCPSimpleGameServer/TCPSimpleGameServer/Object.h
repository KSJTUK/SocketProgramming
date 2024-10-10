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
	Object(Position pos, SIZE boxSize);
	~Object();

public:
	void SetPosition(Position pos);
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

/* ----------------------------------------
*
*				Player
*
  ---------------------------------------- */

class Player : public Object {
	inline static constexpr unsigned int DEFAULT_SIZE = 40;
	inline static constexpr float DEFAULT_SPEED = 300.0f;

public:
	Player();
	Player(float x, float y);
	~Player();

public:
	void SetPosition(const Position& position);
	void SetDirection(char x, char y);
	void SetDirection(Direction2D dir);
	void SetValocity(float velocity);

	// Getter
	Direction2D GetDirection() const { return mDirection; }
	Position GetPosition() const { return mPosition; }
	float GetVelocity() const { return mVelocity; }

	void RegisterExecutionFn();

	// 
	void MoveLeft(float deltaTime);
	void MoveUp(float deltaTime);
	void MoveRight(float deltaTime);
	void MoveDown(float deltaTime);

	void Update(const float deltaTime, byte input);
	void Render();

private:
	// 움직임 관련
	Direction2D mDirection{ };
	float mVelocity{ };
	Position mPosition{ };

	//RECT mViewRect{ };

	std::unique_ptr<class ProcessKeyInput> mInputProcessor;

private:
	virtual void HandleCollision(Object* other);
};