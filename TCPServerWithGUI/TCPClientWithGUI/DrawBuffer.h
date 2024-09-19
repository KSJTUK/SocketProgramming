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
	void SetCameraPosition(Position cameraPosition);
	Position GetCameraPosition() const;
	std::pair<LONG, LONG> GetCameraLeftTop() const;
	HDC GetMemDC() const;

	void DrawString(std::string_view str, int x, int y);
	void CleanupBuffer();
	void CopyBufferMemToMain();

private:
	WindowInfo mWindowInfo{ };
	HDC mMainFrameDC{ };
	HDC mMemDC{ };
	HBITMAP mMemBmp{ };
	RECT mValidBufferRect{ 0, 0, WORLD_SIZE.cx, WORLD_SIZE.cy };
	Position mCameraPosition{ };
};

