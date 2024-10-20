#include "pch.h"
#include "DrawBuffer.h"

DrawBuffer::DrawBuffer() = default;

DrawBuffer::DrawBuffer(const WindowInfo& windowInfo, DWORD bgColor)
	: mWindowInfo{ windowInfo },
	mBackGroundColor{ bgColor }
{
	// ���� DC�� ������ ȣȯ�Ǵ� ����� ����
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

	// ī�޶� ��ġ�� �������� ������ ������ ������ ����
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

// Backbuffer ����
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

// ����� �ȼ� �ʱ�ȭ
void DrawBuffer::CopyBufferMemToMain() const
{
	auto [left, top, right, bottom] = mWindowInfo.windowRect;

	// ī�޶� �������� �׸���, ���� ��Ʈ���� left, top��ǥ�� 0~ ���� ũ�� - right / 2, �� �׸��� ������ ���� ��Ʈ���� ũ�⸦ �Ѿ��ڱ� �ʵ��� ��
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
