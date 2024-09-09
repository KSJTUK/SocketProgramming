#pragma once

/* ----------------------------------------
*					Player
*
*				게임 플레이어. 
  ---------------------------------------- */

class Player {
	inline static constexpr unsigned int DEFAUT_SIZE = 40;

public:
	Player();
	Player(float x, float y);
	~Player();

public:
	// Setter
	void SetPosition(float x, float y);
	void SetPosition(const Position& position);
	void SetDirection(char x, char y);
	void SetDirection(Direction2D dir);
	void SetValocity(float velocity);

	// Getter
	Direction2D GetDirection() const { return mDirection; }
	Position GetPosition() const { return mPosition; }
	float GetVelocity() const { return mVelocity; }

	// 
	void Move();

	void Update();
	void Render();

private:
	// 움직임 관련
	Direction2D mDirection{ };
	float mVelocity{ };
	Position mPosition{ };

	// 플레이어가 그려질 도형정보
	std::unique_ptr<class Shape> mShape{ };
};