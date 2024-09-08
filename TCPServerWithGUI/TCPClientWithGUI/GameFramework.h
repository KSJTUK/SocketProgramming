#pragma once

/* ----------------------------------------
*				GameFramework
* 기본적인 윈도우 생성, 렌더링, 업데이트 함수등
* System Call, 메인 로직등을 담당할 클래스
* 이 클래스의 객체는 특별한 이유가 없다면 
* 단 하나만 만들 것
  ---------------------------------------- */

class GameFramework {
	// 윈도우 메세지 처리 함수 
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	GameFramework();
	~GameFramework();

public:
	bool Init(HINSTANCE instanceHandle);
	void Destroy();

	// 그리기에 필요한 객체들을 생성하는 함수
	void CreateObjects();
	void AddShape(class Shape* shape);

	std::shared_ptr<class DrawBuffer> GetDrawBuffer() const;

	void OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void FrameAdvance();

private:
	void CreateMyWindow();
	void RegisterWindow();

private:
	// instance, window 핸들
	HINSTANCE		mInstanceHandle{ };
	WindowInfo		mWindowInfo{ };

	// 생성된 윈도우 이름, 클래스 이름 저장
	std::wstring	mWindowName{ L"GUI 클라이언트" };
	std::wstring	mWindowClassName{ L"GUI 클라이언트" };

	std::shared_ptr<class DrawBuffer> mDrawBuffer{ };
	std::unique_ptr<class ServerService> mServerService{ };

	// draw test
	std::list<std::unique_ptr<class Shape>> mDrawTestShapes{ };
};