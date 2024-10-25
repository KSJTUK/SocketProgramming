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
	Object(const Vec2D pos, SizeF boxSize, DWORD color = RGB(255, 255, 255), OBJECT_TYPE objType = NONE);
	virtual ~Object();

public:
	void SetColor(const DWORD color);
	void SetPosition(const Vec2D pos);
	void SetSize(const SizeF size);

	void SetActive(bool active);

	OBJECT_TYPE GetType() const;
	Vec2D GetPosition() const;
	SizeF GetBoxSize() const;
	DWORD GetColor() const;
	Vec2D GetDirection() const;
	float GetVelocity() const;

	RECT GetBox() const;

	const class Collider* GetCollider() const;

	void Reset();

public:
	bool CheckCollision(Object* const other);

	virtual void Update(const float deltaTime);
	virtual void HandleCollision(Object* const other) abstract;

private:
	inline static std::atomic_ullong unique_id{ 0 };

private:
	OBJECT_TYPE mObjType;
	DWORD mColor;

	std::unique_ptr<class Collider> mCollider;

	bool mActive;
};

/* ----------------------------------------
*
*				WALL
*
  ---------------------------------------- */

class Wall : public Object {
public:
	Wall() : Object{ WALL } { }
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
	Bullet() : Object{ BULLET } { }
	Bullet(const Vec2D pos, SizeF boxSize, DWORD color) : Object{ pos, boxSize, color, BULLET } { }
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
	Ball() : Object{ BALL } { }
	Ball(const Vec2D pos, SizeF boxSize, DWORD color) : Object{ pos, boxSize, color, BALL } { }
	virtual ~Ball() { }

public:
	virtual void HandleCollision(Object* other);
};