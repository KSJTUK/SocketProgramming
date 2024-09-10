#include "pch.h"
#include "DrawBuffer.h"

DrawBuffer::DrawBuffer() = default;

DrawBuffer::DrawBuffer(const WindowInfo& windowInfo)
	: mWindowInfo{ windowInfo }
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

void DrawBuffer::SetCameraPosition(Position cameraPosition)
{
	CleanupBuffer();
	mCameraPosition = cameraPosition;
}

Position DrawBuffer::GetCameraPosition() const
{
	return mCameraPosition;
}

// Backbuffer 리턴
HDC DrawBuffer::GetMemDC() const
{
	return mMemDC;
}

void DrawBuffer::CleanupBuffer()
{
	RECT worldRect{ 0, 0, WORLD_SIZE.cx, WORLD_SIZE.cy };
	FillRect(mMemDC, &worldRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

// 백버퍼 픽셀 초기화
void DrawBuffer::CopyBufferMemToMain()
{
	auto [left, top, right, bottom] = mWindowInfo.windowRect;

	LONG srcX = std::clamp(static_cast<LONG>(mCameraPosition.x) - right / 2, 0L, WORLD_SIZE.cx - right);
	LONG srcY = std::clamp(static_cast<LONG>(mCameraPosition.y) - bottom / 2, 0L, WORLD_SIZE.cx - bottom);

	BitBlt(mMainFrameDC, 0, 0, right, bottom, mMemDC, srcX, srcY, SRCCOPY);
}
