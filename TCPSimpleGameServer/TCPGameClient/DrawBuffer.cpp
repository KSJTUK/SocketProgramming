#include "pch.h"
#include "DrawBuffer.h"

DrawBuffer::DrawBuffer() = default;

DrawBuffer::DrawBuffer(const WindowInfo& windowInfo, DWORD bgColor)
	: mWindowInfo{ windowInfo },
	mBackGroundColor{ bgColor }
{
	// 메인 DC를 얻어오고 호환되는 백버퍼 생성
	mMainFrameDC = GetDC(mWindowInfo.windowHandle);
	mMemDC = CreateCompatibleDC(mMainFrameDC);
	mMemBmp = CreateCompatibleBitmap(mMainFrameDC, WORLD_SIZE.cx, WORLD_SIZE.cy);
	SelectObject(mMemDC, mMemBmp);
}

DrawBuffer::~DrawBuffer()
{
	DeleteObject(mMemBmp);
	DeleteDC(mMemDC);
	ReleaseDC(mWindowInfo.windowHandle, mMainFrameDC);
}

void DrawBuffer::SetCameraPosition(Vec2D cameraPosition)
{
	mCameraPosition = cameraPosition;

	// 카메라 위치를 기준으로 복사할 월드의 영역을 설정
	auto [left, top] = GetCameraLeftTop();
	mValidBufferRect = RECT{ left, top, left + mWindowInfo.windowRect.right, top + mWindowInfo.windowRect.bottom };
}

void DrawBuffer::SetBackGroundColor(DWORD bgColor)
{
	mBackGroundColor = bgColor;
}

Vec2D DrawBuffer::GetCameraPosition() const
{
	return mCameraPosition;
}

std::pair<LONG, LONG> DrawBuffer::GetCameraLeftTop() const
{
	auto [left, top, right, bottom] = mWindowInfo.windowRect;
	LONG cameraLeft = std::clamp(static_cast<LONG>(mCameraPosition.x) - right / 2, 0L, WORLD_SIZE.cx - right);
	LONG cameraTop = std::clamp(static_cast<LONG>(mCameraPosition.y) - bottom / 2, 0L, WORLD_SIZE.cx - bottom);
	return { cameraLeft, cameraTop };
}

// Backbuffer 리턴
HDC DrawBuffer::GetMemDC() const
{
	return mMemDC;
}

bool DrawBuffer::IsInCamera(Vec2D objectPos) const
{
	return mValidBufferRect.Contains(objectPos);
}

void DrawBuffer::DrawString(std::string_view str, const int x, const int y) const
{
	auto [cameraLeft, cameraTop] = GetCameraLeftTop();
	TextOutA(mMemDC, cameraLeft + x, cameraTop + y, str.data(), static_cast<int>(str.size()));
}

void DrawBuffer::CleanupBuffer()
{
	auto [l, t, r, b] = mValidBufferRect.Rect();
	SetBrush(mBackGroundColor);
	Rectangle(mMemDC, l, t, r, b);
	ResetBrush();
}

// 백버퍼 픽셀 초기화
void DrawBuffer::CopyBufferMemToMain() const
{
	auto [left, top, right, bottom] = mWindowInfo.windowRect;

	// 카메라를 기준으로 그리기, 원본 비트맵의 left, top좌표가 0~ 월드 크기 - right / 2, 즉 그리는 영역이 원본 비트맵의 크기를 넘어자기 않도록 함
	auto [srcX, srcY] = GetCameraLeftTop();

	BitBlt(mMainFrameDC, 0, 0, right, bottom, mMemDC, srcX, srcY, SRCCOPY);
}

void DrawBuffer::SetBrush(DWORD color)
{
	mOwnBrush = reinterpret_cast<HBRUSH>(SelectObject(mMemDC, CreateSolidBrush(color)));
}

void DrawBuffer::ResetBrush() const
{
	HBRUSH oldBrush = reinterpret_cast<HBRUSH>(SelectObject(mMemDC, mOwnBrush));
	DeleteObject(oldBrush);
}
