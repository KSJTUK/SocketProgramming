#pragma once

/* ----------------------------------------
*					Player
*
*				���� �÷��̾�.
  ---------------------------------------- */

class Player {
    inline static constexpr float DEFAULT_SIZE = 40.0f;
    inline static constexpr float DEFAULT_SPEED = 300.0f;

public:
    Player();
    Player(float x, float y);
    Player(const Vec2D pos);
    ~Player();

public:
    // Setter
    void SetPosition(const Vec2D& position);
    void SetDirection(float  x, float y);
    void SetDirection(Vec2D dir);
    void SetValocity(float velocity);

    // Getter
    Vec2D GetDirection() const { return mDirection; }
    Vec2D GetPosition() const { return mPosition; }
    float GetVelocity() const { return mVelocity; }

    void Update(const float deltaTime);
    void Render();

private:
    // ������ ����
    Vec2D mDirection{ };
    SizeF mSize{ };
    DWORD mColor{ RGB(0, 255, 0) };
    float mVelocity{ };
    Vec2D mPosition{ };

    bool mPlayable{ false };

    // �÷��̾ �׷��� ��������
    std::string mName{ "Position" };
    std::shared_ptr<Shape> mShape{ };
};