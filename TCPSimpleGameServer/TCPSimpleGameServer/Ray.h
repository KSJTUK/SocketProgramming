#pragma once

#undef min
#undef max

struct Ray {
    Ray(const Vec2D& origin, const Vec2D& terminal) : origin{ origin }, direction{ Vec2D::GetNormalized(terminal - origin) } { }
    ~Ray() { }

    Vec2D origin;
    Vec2D direction;
};

inline int CCW(const Vec2D& p1, const Vec2D& p2, const Vec2D& p3)
{
    return static_cast<int>((p1.x * p2.y + p2.x * p3.y + p3.x * p1.x) - (p2.x * p1.y + p3.x * p2.y + p1.x * p3.y));
}

inline bool IsCCW(const Vec2D& p1, const Vec2D& p2, const Vec2D& p3)
{
    return CCW(p1, p2, p3) > 0;
}

inline Vec2D CCWLineNormal(const Vec2D& lineA, const Vec2D& lineB)
{
    Vec2D dirVec = lineB - lineA;
    dirVec.Normalize();

    return Vec2D{ -dirVec.Y(), dirVec.X() };
}

inline Vec2D Reflect(const Vec2D& incident, const Vec2D& planeA, const Vec2D& planeB)
{
    Vec2D normal = CCWLineNormal(planeA, planeB);
    float dotResult = incident.Dot(normal);

    Vec2D reflectionVec = incident - (normal * (2 * dotResult));
    return reflectionVec;
}

inline bool LineIntersect(const Vec2D& rayStart, const Vec2D& rayEnd, const Vec2D& planeA, const Vec2D& planeB)
{
    int AB = CCW(planeA, planeB, rayStart) * CCW(planeA, planeB, rayEnd);
    int RAY = CCW(rayStart, rayEnd, planeA) * CCW(rayStart, rayEnd, planeB);

    if ((AB == 0 and RAY == 0) or (AB > 0 or RAY > 0)) {
        return false;
    }

    return true;
}

inline Collider::PLANE_DIR NearlestPlane(const Ray& ray, const Collider& collider)
{
    // 각이 45도이면 그냥 제일 먼저나오는 면이 선택되는 문제가 있음
    std::array<float, Collider::PLANE_DIR_COUNT> dotResults;
    Vec2D dir = Vec2D::GetNormalized(collider.GetOldPosition() - ray.origin);
    for (int planeDir = 0; planeDir < Collider::PLANE_DIR_COUNT; ++planeDir) {
        auto [lineStart, lineEnd] = collider.Plane(static_cast<Collider::PLANE_DIR>(planeDir));
        auto planeNormal = CCWLineNormal(lineStart, lineEnd);

        dotResults[planeDir] = dir.Dot(planeNormal);
    }

    auto minIndex = std::distance(dotResults.begin(), std::min_element(dotResults.begin(), dotResults.end()));

    return static_cast<Collider::PLANE_DIR>(minIndex);
}

inline void SlidingVector(OUT Vec2D& slidingVec, const Vec2D& pos, const Vec2D& oldPos, const Collider& collider)
{
    Vec2D moveVec = pos - oldPos;
    auto nearlestPlane = NearlestPlane(Ray{ oldPos, pos }, collider);
    auto [lineA, lineB] = collider.Plane(nearlestPlane);
    Vec2D planeNormal = CCWLineNormal(lineA, lineB);

    float dotResult = Vec2D::Dot(planeNormal, moveVec);
    slidingVec = moveVec - dotResult * planeNormal;
}

inline void SlidingVector(OUT Vec2D& slidingVec, const Collider& collider, const Collider& otherCollider)
{
    SlidingVector(slidingVec, collider.GetPosition(), collider.GetOldPosition(), otherCollider);
}
