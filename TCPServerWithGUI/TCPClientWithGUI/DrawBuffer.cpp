#include "pch.h"
#include "DrawBuffer.h"

DrawBuffer::DrawBuffer() = default;

DrawBuffer::DrawBuffer(const WindowInfo& windowInfo)
	: mWindowInfo{ windowInfo }
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

void DrawBuffer::SetCameraPosition(Position cameraPosition)
{
	mCameraPosition = cameraPosition;

	// ī�޶� ��ġ�� �������� ������ ������ ������ ����
	auto [left, top] = GetCameraLeftTop();
	mValidBufferRect = { left, top, left + mWindowInfo.windowRect.right, top + mWindowInfo.windowRect.bottom };
}

Position DrawBuffer::GetCameraPosition() const
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

void DrawBuffer::DrawString(std::string_view str, int x, int y)
{
	auto [cameraLeft, cameraTop] = GetCameraLeftTop();
	TextOutA(mMemDC, cameraLeft + x, cameraTop + y, str.data(), static_cast<int>(str.size()));
}

void DrawBuffer::CleanupBuffer()
{
	FillRect(mMemDC, &mValidBufferRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
}

// ����� �ȼ� �ʱ�ȭ
void DrawBuffer::CopyBufferMemToMain()
{
	auto [left, top, right, bottom] = mWindowInfo.windowRect;

	// ī�޶� �������� �׸���, ���� ��Ʈ���� left, top��ǥ�� 0~ ���� ũ�� - right / 2, �� �׸��� ������ ���� ��Ʈ���� ũ�⸦ �Ѿ��ڱ� �ʵ��� ��
	auto [srcX, srcY] = GetCameraLeftTop();

	BitBlt(mMainFrameDC, 0, 0, right, bottom, mMemDC, srcX, srcY, SRCCOPY);
}
