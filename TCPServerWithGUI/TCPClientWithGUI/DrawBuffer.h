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

	void Render();

private:
	WindowInfo mWindowInfo{ };
	HDC mMainFrameDC{ };
	HDC mMemDC{ };
	HBITMAP mMemBmp{ };
};

