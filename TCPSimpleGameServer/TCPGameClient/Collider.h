#pragma once

class Collider {
public:
    enum PLANE_DIR {
        LEFT,
        TOP,
        RIGHT,
        BOTTOM,
        PLANE_DIR_COUNT,
        NONE,
    };

public:
    Collider();
    ~Collider();

    Collider(const Collider& other);

    Collider(const Vec2D& center, float w, float h);
    Collider(const Vec2D& center, const SizeF& size);

    Collider(float l, float t, float r, float b);
    Collider(const RECT& rect);

public:
    // Getter
    // rect 관련 Getter, Setter
    float Left() const;
    float Top() const;
    float Right() const;
    float Bottom() const;

    Vec2D LeftTop() const;
    Vec2D LeftBottom() const;
    Vec2D RightTop() const;
    Vec2D RightBottom() const;

    SizeF GetSize() const;

    RECT Rect() const;

    void SetSize(const SizeF& size);

    // 움직임 관련 Getter, Setter
    Vec2D GetPosition() const;
    Vec2D GetDirection() const;
    Vec2D GetOldPosition() const;
    float GetVelocity() const;

    Vec2D GetMoveVec() const;

    void SetVelocity(float velocity);
    void SetPosition(const Vec2D& pos, bool updateOld = false);
    void SetDirection(const Vec2D& dir);

public:
    // 충돌 처리 관련 함수들
    bool IsEmptyArea() const;

    bool Intersect(const Collider& b) const;
    void Offset(const Vec2D& moveVec);

    Collider MergedBox(const Collider& otherBox) const;
    void Union(const Collider& otherBox);

    bool Contains(const Vec2D& vec) const;

    std::pair<Vec2D, Vec2D> Plane(PLANE_DIR dir) const;

public:
    // 업데이트
    void Update(float deltaTime);

    void Render(const std::shared_ptr<class DrawBuffer>& drawBuffer);

public:
    static bool Intersect(Collider& intersectRect, const Collider& a, const Collider& b);
    static Collider Merge(const Collider& b1, const Collider& b2);

private:
    Vec2D mPosition;
    Vec2D mOldPosition;
    Vec2D mDirection;

    SizeF mSize;
    float mVelocity;
};

