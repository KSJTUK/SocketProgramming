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
	inline static std::atomic<unsigned long long> mRecvTimeLatency = 0;

public:
	GameFramework();
	~GameFramework();

public:
	bool Init(HINSTANCE instanceHandle);
	void Destroy();

	// 키보드 마우스 캡쳐 (여러 프로세스가 동시에 입력받는 걸 방지)
	void SetKeyboardFocuse(bool focused);
	void SetMouseCapture(bool captured);

	// 그리기에 필요한 객체들을 생성하는 함수
	void CreateObjects();

	// 다른 플레이어 접속, 퇴장, 업데이트 처리
	void JoinOtherPlayer(byte id, class Player* player);
	void UpdateJoinedPlayer(byte id, Position pos);
	void ExitPlayer(byte id);

	// 핑에 대한 결과를 받는 함수
	void PingResult(std::chrono::high_resolution_clock::time_point timeSent);

	// Getter
	HWND GetWindowHandle() const { return mWindowInfo.windowHandle; }
	std::shared_ptr<class KeyInput> GetKeyInput() const;
	std::shared_ptr<class DrawBuffer> GetDrawBuffer() const;

	// 윈도우 메시지 처리
	void OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// 업데이트와 렌더링 부분 분리
	void Update();
	void Render();

	// 전체 프레임 작업 수행
	void FrameAdvance();

private:
	void CreateMyWindow();
	void RegisterWindow();

private:
	// instance, window 핸들
	HINSTANCE		mInstanceHandle{ };
	WindowInfo		mWindowInfo{ };

	bool mKeyboardFocused{ false };
	bool mMouseCaptured{ false };

	// 생성된 윈도우 이름, 클래스 이름 저장
	std::wstring	mWindowName{ L"GUI 클라이언트" };
	std::wstring	mWindowClassName{ L"GUI 클라이언트" };

	// 타이머
	std::unique_ptr<class Timer> mTimer{ };

	// 렌더링 처리
	std::shared_ptr<class DrawBuffer> mDrawBuffer{ };

	// 키입력 처리
	std::shared_ptr<class KeyInput> mKeyInput{ };

	// 서버 데이터 송수신 처리
	std::unique_ptr<class ServerService> mServerService{ };

	// draw test
	std::unique_ptr<class Player> mPlayer{ };
	std::unordered_map<byte, std::unique_ptr<class Player>> mOtherPlayers{ };
};