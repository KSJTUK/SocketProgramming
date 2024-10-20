#pragma once

#undef max
#undef min
#include <utility>

struct SizeF {
    float w;
    float h;
};

class Vec2D {
public:
    Vec2D() : x{ 0.0f }, y{ 0.0f } { }
    ~Vec2D() { }

    Vec2D(float x, float y) : x{ x }, y{ y } { }
    Vec2D(const Vec2D& other) : x{ other.x }, y{ other.y } { }
    Vec2D(Vec2D&& other) noexcept : x{ other.x }, y{ other.y } { }
    Vec2D& operator=(const Vec2D& other) { x = other.x; y = other.y; return *this; }
    void operator=(Vec2D&& other) noexcept { x = other.x; y = other.y; }

public:
    // 연산자 오버로딩
    Vec2D operator+(const Vec2D& other) const
    {
        return Vec2D{ x + other.x, y + other.y };
    }

    Vec2D operator-(const Vec2D& other) const
    {
        return Vec2D{ x - other.x, y - other.y };
    }

    Vec2D operator*(float scalar) const
    {
        return Vec2D{ x * scalar, y * scalar };
    }

    Vec2D operator/(float scalar) const
    {
        return Vec2D{ x / scalar, y / scalar };
    }

    void operator+=(const Vec2D& other)
    {
        x += other.x; y += other.y;
    }

    void operator-=(const Vec2D& other)
    {
        x -= other.x; y -= other.y;
    }

    void operator*=(float scalar)
    {
        x *= scalar; y *= scalar;
    }

    void operator/=(float scalar)
    {
        x /= scalar; y /= scalar;
    }

    friend Vec2D operator*(float scalar, const Vec2D& vec)
    {
        return (vec * scalar);
    }

public:
    float Dot(const Vec2D& other) const
    {
        return (x * other.x) + (y * other.y);
    }

    float Cross(const Vec2D& other) const
    {
        return ((x * other.y) - (other.x * y));
    }

    float Length() const
    {
        return std::sqrtf((x * x) + (y * y));
    }

    Vec2D GetNormalizedVec() const
    {
        float length = Length();
        return (*this) / length;
    }

    void Normalize() { (*this) /= Length(); }

    float X() const { return x; }
    float Y() const { return y;  }

public:
    static float Dot(const Vec2D& a, const Vec2D& b)
    {
        return (a.x * b.x) + (a.y * b.y);
    }

    static float Cross(const Vec2D& a, const Vec2D& b)
    {
        return (a.x * b.y) - (b.x * a.y);
    }

    static Vec2D GetNormalized(const Vec2D& vec)
    {
        return vec / vec.Length();
    }

    static void Normalize(Vec2D& vec)
    {
        vec.Normalize();
    }

    static float Length(const Vec2D& vec)
    {
        return vec.Length();
    }

public:
    float x;
    float y;
};