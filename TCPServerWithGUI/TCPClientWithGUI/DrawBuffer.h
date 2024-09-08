#pragma once

/* ----------------------------------------
*
*				DrawBuffer
*
  ---------------------------------------- */

class DrawBuffer {
public:
	DrawBuffer();
	DrawBuffer(const WindowInfo& windowInfo);
	~DrawBuffer();

public:
	HDC GetMemDC() const;

	void CleanupBuffer();
	void CopyBufferMemToMain();

private:
	WindowInfo mWindowInfo{ };
	HDC mMainFrameDC{ };
	HDC mMemDC{ };
	HBITMAP mMemBmp{ };
};

