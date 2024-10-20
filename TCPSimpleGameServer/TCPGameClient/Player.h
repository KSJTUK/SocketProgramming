#pragma once

/* ----------------------------------------
*					Player
*
*				게임 플레이어.
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
    // 움직임 관련
    Vec2D mDirection{ };
    SizeF mSize{ };
    DWORD mColor{ RGB(0, 255, 0) };
    float mVelocity{ };
    Vec2D mPosition{ };

    bool mPlayable{ false };

    // 플레이어가 그려질 도형정보
    std::string mName{ "Position" };
    std::shared_ptr<Shape> mShape{ };
};