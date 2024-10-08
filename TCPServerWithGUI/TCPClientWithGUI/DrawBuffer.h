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

	bool IsInCamera(Position objectPos) const;

	void DrawString(std::string_view str, const int x, const int y) const;
	void CleanupBuffer() const;
	void CopyBufferMemToMain() const;

private:
	WindowInfo mWindowInfo{ };
	HDC mMainFrameDC{ };
	HDC mMemDC{ };
	HBITMAP mMemBmp{ };
	RECT mValidBufferRect{ 0, 0, WORLD_SIZE.cx, WORLD_SIZE.cy };
	Position mCameraPosition{ };
};

