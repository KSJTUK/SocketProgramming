#pragma once

/* ----------------------------------------
*
*				DrawBuffer
*
  ---------------------------------------- */

class DrawBuffer {
public:
	DrawBuffer(const WindowInfo& windowInfo);
	~DrawBuffer();

public:
	void Render();

private:
	WindowInfo mWindowInfo{ };
	HDC mMainFrameDC{ };
	HDC mBackBufferDC{ };
};

