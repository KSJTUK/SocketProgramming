#pragma once

/* ----------------------------------------
*					Player
*
*				게임 플레이어. 
  ---------------------------------------- */

class Player {
	inline static constexpr Direction2D DIRECTIONS[4]{
		/* dx, dy */
		{ -1, 0 }, /* LEFT */
		{ 0, -1}, /* UP */
		{ 1, 0 }, /* RIGHT */
		{ 0, 1}, /* DOWN */
	};

	enum {
		LEFT,
		UP,
		RIGHT,
		DOWN
	};

	inline static constexpr unsigned int DEFAUT_SIZE = 40;
	inline static constexpr float DEFAULT_SPEED = 30.0f;

public:
	Player();
	Player(bool playable);
	Player(float x, float y);
	Player(float x, float y, bool playable);
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
	void Move(const float deltaTime);
	void MoveLeft();
	void MoveUp();
	void MoveRight();
	void MoveDown();

	void Update(const float deltaTime);
	void Render();

private:
	// 움직임 관련
	Direction2D mDirection{ };
	float mVelocity{ };
	Position mPosition{ };

	bool mPlayable{ false };

	// 플레이어가 그려질 도형정보
	std::string mName{ "Position" };
	std::unique_ptr<class Shape> mShape{ };
};