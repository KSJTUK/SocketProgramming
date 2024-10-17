#pragma once

#undef max
#undef min
#include <utility>

struct Position {
	float x;
	float y;
};

struct Direction2D {
	char dx;
	char dy;
};

// 사용 전 Windows.h 포함
// 위 헤더를 포함했다고 가정하고 만듦
// 기존 RECT를 확장한 헬퍼 클래스
class RECTEX {
public:
	/* ----------------------Constructor---------------------- */
	RECTEX()
		: rect{ 0, 0, 0, 0 }
	{
	}

	RECTEX(long centerX, long centerY, long width, long height)
	{
		rect = RECT{ 
			centerX - width / 2, centerY - height / 2,
			centerX + width / 2, centerY + height / 2 
		};
	}

	RECTEX(const Position pos, const SIZE size)
		: rect{ 
		static_cast<long>(pos.x) - size.cx / 2, 
		static_cast<long>(pos.x) - size.cy / 2,
		static_cast<long>(pos.x) + size.cx / 2,
		static_cast<long>(pos.x) + size.cy / 2 
		}
	{
	}

	RECTEX(const RECTEX& other) 
		: rect{ other.rect }
	{	
	}

	RECTEX& operator=(const RECTEX& other)
	{
		rect = other.rect;
		return *this;
	}

	RECTEX& operator=(const RECT& otherRC)
	{
		rect = otherRC;
		return *this;
	}

	bool operator==(const RECTEX& other) const
	{
		return (
			rect.left == other.rect.left and
			rect.top == other.rect.top and
			rect.right == other.rect.right and
			rect.bottom == other.rect.bottom
		);
	}

	std::pair<long, long> Center() const
	{
		return std::make_pair((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
	}

	std::pair<long, long> LeftTop() const
	{
		return std::make_pair(rect.left, rect.top);
	}
	
	std::pair<long, long> RightBottom() const
	{
		return std::make_pair(rect.right, rect.bottom);
	}

	std::pair<long, long> Size() const
	{
		return std::make_pair(rect.right - rect.left, rect.bottom - rect.top);
	}

	bool IsEmptyArea() const
	{
		auto [w, h] = Size();

		return ((w <= 0) or (h <=0 ));
	}

	bool Contains(Position pos) const
	{
		int x = static_cast<long>(pos.x);
		int y = static_cast<long>(pos.y);

		return (
			x <= rect.right and
			x >= rect.left and
			y <= rect.bottom and
			y >= rect.top
			);
	}

	bool Contains(int x, int y) const
	{
		return (
			x <= rect.right and
			x >= rect.left and
			y <= rect.bottom and
			y >= rect.top
			);
	}

	void Offset(const Position pos)
	{
		rect.left += static_cast<long>(pos.x);
		rect.right += static_cast<long>(pos.x);
		rect.top += static_cast<long>(pos.y);
		rect.bottom += static_cast<long>(pos.y);
	}

	bool ClientRect(HWND hwnd)
	{
		return ::GetClientRect(hwnd, &rect);
	}

	RECT GetRect()
	{
		return rect;
	}

	bool IntersectWith(const RECTEX& other) const
	{
		return (
			other.rect.right < rect.left or
			other.rect.left > rect.right or
			other.rect.top > rect.bottom or
			other.rect.bottom < rect.top
			);
	}

	static bool Intersect(RECTEX& intersectArea, const RECTEX& rectA, const RECTEX& rectB)
	{
		auto [la, ta, ra, ba] = rectA.rect;
		auto [lb, tb, rb, bb] = rectB.rect;

		intersectArea = RECTEX{ 
			std::max(la, lb), 
			std::max(ta, tb), 
			std::min(ra, rb), 
			std::min(ba, bb) 
		};

		return not intersectArea.IsEmptyArea();
	}

	static std::pair<bool, RECTEX> Intersect(const RECTEX& rectA, const RECTEX& rectB)
	{
		RECTEX resultRC;
		bool intersect = Intersect(resultRC, rectA, rectB);
		return std::make_pair(intersect, resultRC);
	}

	static bool Intersect(RECTEX& intersectArea, const Position posA, const SIZE sizeA, const Position posB, const SIZE sizeB)
	{
		return Intersect(intersectArea, RECTEX{ posA, sizeA }, RECTEX{ posB, sizeB });
	}

	static std::pair<bool, RECTEX> Intersect(const Position posA, const SIZE sizeA, const Position posB, const SIZE sizeB)
	{
		RECTEX resultRC;
		bool intersect = Intersect(resultRC, posA, sizeA, posB, sizeB);
		return std::make_pair(intersect, resultRC);
	}

	static bool Union(RECTEX& resultRect, const RECTEX& rectA, const RECTEX& rectB)
	{
		auto [la, ta, ra, ba] = rectA.rect;
		auto [lb, tb, rb, bb] = rectB.rect;

		resultRect = RECTEX{
			std::min(la, lb),
			std::min(ta, tb),
			std::max(ra, rb),
			std::max(ba, bb)
		};

		return not resultRect.IsEmptyArea();
	}

private:
	RECT rect;
};