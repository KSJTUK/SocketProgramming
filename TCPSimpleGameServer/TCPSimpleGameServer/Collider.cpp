#include "pch.h"
#include "Collider.h"

#undef min
#undef max

Collider::Collider() : mPosition{ }, mOldPosition{ mPosition }, mSize{ }, mVelocity{ } { }

Collider::~Collider() { }

Collider::Collider(const Collider& other) : mPosition{ other.mPosition }, mSize{ other.mSize }, mOldPosition{ mPosition }, mVelocity{ } { }

Collider::Collider(const Vec2D& center, float w, float h) : mPosition{ center }, mSize{ w, h }, mOldPosition{ mPosition }, mVelocity{ } { }

Collider::Collider(const Vec2D& center, const SizeF& size) : mPosition{ center }, mSize{ size }, mOldPosition{ mPosition }, mVelocity{ } { }

Collider::Collider(float l, float t, float r, float b)
    : mSize{ r - l, b - t },
    mPosition{ l + (r - l) / 2.0f, t + (b - t) / 2.0f },
    mOldPosition{ mPosition },
    mDirection{ 0.0f, 0.0f },
    mVelocity{ }
{
}

Collider::Collider(const RECT& rect)
    : mSize{ static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top) },
    mPosition{ static_cast<float>(rect.left) + (static_cast<float>(rect.right - rect.left) / 2.0f),
    static_cast<float>(rect.top) + (static_cast<float>(rect.bottom - rect.top) / 2.0f) },
    mOldPosition{ mPosition },
    mDirection{ 0.0f, 0.0f },
    mVelocity{ }
{
}

float Collider::Left() const
{
    return mPosition.X() - mSize.w / 2.0f;
}

float Collider::Top() const
{
    return mPosition.Y() - mSize.h / 2.0f;
}

float Collider::Right() const
{
    return mPosition.X() + mSize.w / 2.0f;
}

float Collider::Bottom() const
{
    return mPosition.Y() + mSize.h / 2.0f;
}

Vec2D Collider::LeftTop() const
{
    return Vec2D{ Left(), Top() };
}

Vec2D Collider::LeftBottom() const
{
    return Vec2D{ Left(), Bottom() };
}

Vec2D Collider::RightTop() const
{
    return Vec2D{ Right(), Top() };
}

Vec2D Collider::RightBottom() const
{
    return Vec2D{ Right(), Bottom() };
}

SizeF Collider::GetSize() const
{
    return mSize;
}

RECT Collider::Rect() const
{
    return RECT{
        static_cast<long>(Left()),
        static_cast<long>(Top()),
        static_cast<long>(Right()),
        static_cast<long>(Bottom())
    };
}

void Collider::SetVelocity(float velocity)
{
    mVelocity = velocity;
}

void Collider::SetPosition(const Vec2D& pos, bool updateOld)
{
    if (updateOld) mOldPosition = mPosition;
    mPosition = pos;
}

void Collider::SetDirection(const Vec2D& dir)
{
    mDirection = dir;
}

void Collider::SetSize(const SizeF& size)
{
    mSize = size;
}

Vec2D Collider::GetPosition() const
{
    return mPosition;
}

Vec2D Collider::GetDirection() const
{
    return mDirection;
}

Vec2D Collider::GetOldPosition() const
{
    return mOldPosition;
}

float Collider::GetVelocity() const
{
    return mVelocity;
}

Vec2D Collider::GetMoveVec() const
{
    return mPosition - mOldPosition;
}

bool Collider::IsEmptyArea() const
{
    return (mSize.w <= 0.0f or mSize.h <= 0.0f);
}

bool Collider::Intersect(const Collider& b) const
{
    return not (
        Left() > b.Right() or
        Right() < b.Left() or
        Top() > b.Bottom() or
        Bottom() < b.Top()
        );
}

void Collider::Offset(const Vec2D& moveVec)
{
    mPosition += moveVec;
}

Collider Collider::MergedBox(const Collider& otherBox) const
{
    float l = std::min(otherBox.Left(), Left());
    float t = std::min(otherBox.Top(), Top());
    float r = std::max(otherBox.Right(), Right());
    float b = std::max(otherBox.Bottom(), Bottom());

    return Collider{ l, t, r, b };
}

void Collider::Union(const Collider& otherBox)
{
    float l = std::min(otherBox.Left(), Left());
    float t = std::min(otherBox.Top(), Top());
    float r = std::max(otherBox.Right(), Right());
    float b = std::max(otherBox.Bottom(), Bottom());

    (*this) = Collider{ l, t, r, b };
}

bool Collider::Contains(const Vec2D& vec) const
{
    return not (
        Left() > vec.X() or
        Right() < vec.X() or
        Top() > vec.Y() or
        Bottom() < vec.Y()
        );
}

std::pair<Vec2D, Vec2D> Collider::Plane(PLANE_DIR dir) const
{
    switch (dir) {
    case LEFT:
        return std::make_pair(LeftTop(), LeftBottom());

    case TOP:
        return std::make_pair(RightTop(), LeftTop());

    case RIGHT:
        return std::make_pair(RightBottom(), RightTop());

    case BOTTOM:
        return std::make_pair(LeftBottom(), RightBottom());
    }

    return std::pair<Vec2D, Vec2D>{ };
}

void Collider::Update(float deltaTime)
{
    mOldPosition = mPosition;
    mPosition += mDirection * mVelocity * deltaTime;
}

void Collider::Render(const std::shared_ptr<class DrawBuffer>& drawBuffer)
{
}

bool Collider::Intersect(Collider& intersectRect, const Collider& b1, const Collider& b2)
{
    float l = std::max(b1.Left(), b2.Left());
    float t = std::max(b1.Top(), b2.Top());
    float r = std::min(b1.Right(), b2.Right());
    float b = std::min(b1.Bottom(), b2.Bottom());

    intersectRect = Collider{ l, t, r, b };

    return not intersectRect.IsEmptyArea();
}

Collider Collider::Merge(const Collider& b1, const Collider& b2)
{
    float l = std::min(b1.Left(), b2.Left());
    float t = std::min(b1.Top(), b2.Top());
    float r = std::max(b1.Right(), b2.Right());
    float b = std::max(b1.Bottom(), b2.Bottom());

    return Collider{ l, t, r, b };
}
