#pragma once

class Player {
public:
	Player();
	Player(float x, float y);
	~Player();

public:
	void SetPosition(float x, float y);
	void SetPosition(const Position& position);
	void SetDirection(char x, char y);
	void SetDirection(Direction2D dir);
	void SetValocity(float velocity);

	Direction2D GetDirection() const { return mDirection; }
	Position GetPosition() const { return mPosition; }
	float GetVelocity() const { return mVelocity; }

	void Move();

	void Update();
	void Render();

private:
	Direction2D mDirection{ };

	float mVelocity{ };
	Position mPosition{ };
	std::unique_ptr<class Shape> mShape{ };
};