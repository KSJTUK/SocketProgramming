#include "pch.h"
#include "DrawBuffer.h"

DrawBuffer::DrawBuffer() = default;

DrawBuffer::DrawBuffer(const WindowInfo& windowInfo)
	: mWindowInfo{ windowInfo }
{
	// ���� DC�� ������ ȣȯ�Ǵ� ����� ����
	mMainFrameDC = GetDC(mWindowInfo.windowHandle);
	mMemDC = CreateCompatibleDC(mMainFrameDC);
	mMemBmp = CreateCompatibleBitmap(mMainFrameDC, windowInfo.windowRect.right, windowInfo.windowRect.bottom);
	SelectObject(mMemDC, mMemBmp);
}

DrawBuffer::~DrawBuffer()
{
	DeleteObject(mMemBmp);
	DeleteDC(mMemDC);
	ReleaseDC(mWindowInfo.windowHandle, mMainFrameDC);
}

// Backbuffer ����
HDC DrawBuffer::GetMemDC() const
{
	return mMemDC;
}

void DrawBuffer::CleanupBuffer()
{
	auto [left, top, right, bottom] = mWindowInfo.windowRect;
	FillRect(mMemDC, &mWindowInfo.windowRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

// ����� �ȼ� �ʱ�ȭ
void DrawBuffer::CopyBufferMemToMain()
{
	auto [left, top, right, bottom] = mWindowInfo.windowRect;
	BitBlt(mMainFrameDC, 0, 0, right, bottom, mMemDC, 0, 0, SRCCOPY);
}
