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
	void SetCameraPosition(Vec2D cameraPosition);
	void SetBackGroundColor(DWORD bgColor);

	Vec2D GetCameraPosition() const;
	std::pair<LONG, LONG> GetCameraLeftTop() const;
	HDC GetMemDC() const;

	bool IsInCamera(Vec2D objectPos) const;

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
	Collider mValidBufferRect{ 0.0f, 0.0f, static_cast<float>(WORLD_SIZE.cx), static_cast<float>(WORLD_SIZE.cy) };
	Vec2D mCameraPosition{ };
};

