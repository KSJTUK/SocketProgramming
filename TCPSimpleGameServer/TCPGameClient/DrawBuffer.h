#pragma once

/* ----------------------------------------
*
*				DrawBuffer
*
  ---------------------------------------- */

class DrawBuffer {
public:
	DrawBuffer();
	DrawBuffer(const WindowInfo& windowInfo, DWORD bgColor);
	~DrawBuffer();

public:
	void SetCameraPosition(Position cameraPosition);
	void SetBackGroundColor(DWORD bgColor);

	Position GetCameraPosition() const;
	std::pair<LONG, LONG> GetCameraLeftTop() const;
	HDC GetMemDC() const;

	bool IsInCamera(Position objectPos) const;

	void DrawString(std::string_view str, const int x, const int y) const;
	void CleanupBuffer();
	void CopyBufferMemToMain() const;

	void SetBrush(DWORD color);
	void ResetBrush() const;

private:
	// Handle
	HDC mMainFrameDC{ };
	HDC mMemDC{ };
	HBITMAP mMemBmp{ };
	HBRUSH mOwnBrush{ nullptr };

	DWORD mBackGroundColor{ 0 };
	WindowInfo mWindowInfo{ };
	RECTEX mValidBufferRect{ 0, 0, WORLD_SIZE.cx, WORLD_SIZE.cy };
	Position mCameraPosition{ };
};

