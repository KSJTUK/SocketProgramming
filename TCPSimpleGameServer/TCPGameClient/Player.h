#pragma once

/* ----------------------------------------
*					Player
*
*				게임 플레이어.
  ---------------------------------------- */

class Player {
	inline static constexpr unsigned int DEFAULT_SIZE = 40;
	inline static constexpr float DEFAULT_SPEED = 300.0f;

public:
	Player();
	Player(float x, float y);
	Player(const Position pos);
	~Player();

public:
	// Setter
	void SetPosition(const Position& position);
	void SetDirection(char x, char y);
	void SetDirection(Direction2D dir);
	void SetValocity(float velocity);

	// Getter
	Direction2D GetDirection() const { return mDirection; }
	Position GetPosition() const { return mPosition; }
	float GetVelocity() const { return mVelocity; }

	void Update(const float deltaTime);
	void Render();

private:
	// 움직임 관련
	Direction2D mDirection{ };
	SIZE mSize{ };
	float mVelocity{ };
	Position mPosition{ };

	bool mPlayable{ false };

	// 플레이어가 그려질 도형정보
	std::string mName{ "Position" };
	std::shared_ptr<Shape> mShape{ };
};