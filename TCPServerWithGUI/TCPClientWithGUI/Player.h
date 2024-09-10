#pragma once

/* ----------------------------------------
*					Player
*
*				���� �÷��̾�. 
  ---------------------------------------- */

class Player {
	inline static constexpr Direction2D DIRECTIONS[4]{
		/* dx, dy */
		{ -1, 0 }, /* LEFT */
		{ 0, -1}, /* UP */
		{ 1, 0 }, /* RIGHT */
		{ 0, 1}, /* DOWN */
	};

	inline static constexpr unsigned int DEFAUT_SIZE = 40;
	enum {
		LEFT,
		UP,
		RIGHT,
		DOWN
	};

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
	void Move();
	void MoveLeft();
	void MoveUp();
	void MoveRight();
	void MoveDown();

	void Update();
	void Render();

private:
	// ������ ����
	Direction2D mDirection{ };
	float mVelocity{ };
	Position mPosition{ };

	bool mPlayable{ false };

	// �÷��̾ �׷��� ��������
	std::string mName{ "Position" };
	std::unique_ptr<class Shape> mShape{ };
};