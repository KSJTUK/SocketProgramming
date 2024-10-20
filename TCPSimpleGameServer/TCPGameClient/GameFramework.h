#pragma once

/* ----------------------------------------
*				GameFramework
* �⺻���� ������ ����, ������, ������Ʈ �Լ���
* System Call, ���� �������� ����� Ŭ����
* �� Ŭ������ ��ü�� Ư���� ������ ���ٸ�
* �� �ϳ��� ���� ��
  ---------------------------------------- */

class GameFramework {
	// ������ �޼��� ó�� �Լ� 
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	inline static std::atomic<unsigned long long> mRecvTimeLatency = 0;

public:
	GameFramework();
	~GameFramework();

public:
	bool Init(HINSTANCE instanceHandle);
	void Destroy();

	// Ű���� ���콺 ĸ�� (���� ���μ����� ���ÿ� �Է¹޴� �� ����)
	void SetKeyboardFocuse(bool focused);
	void SetMouseCapture(bool captured);

	// �׸��⿡ �ʿ��� ��ü���� �����ϴ� �Լ�
	void CreateObjects();

	// �ٸ� �÷��̾� ����, ����, ������Ʈ ó��
	void JoinOtherPlayer(const byte id, const Vec2D pos);
	void UpdateJoinedPlayer(const byte id, const Vec2D pos);
	void ExitPlayer(const byte id);

	// �ο� ���� ����� �޴� �Լ�
	void PingResult(std::chrono::high_resolution_clock::time_point timeSent);

	// Getter
	HWND GetWindowHandle() const { return mWindowInfo.windowHandle; }
	std::shared_ptr<class KeyInput> GetKeyInput() const;
	std::shared_ptr<class DrawBuffer> GetDrawBuffer() const;

	// ������Ʈ ����, ������Ʈ
	void UpdateObject(PacketObjectInfo* objectInfo);

	// ������ �޽��� ó��
	void OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// ������Ʈ�� ������ �κ� �и�
	void Update();
	void Render();

	// ��ü ������ �۾� ����
	void FrameAdvance();

private:
	void CreateMyWindow();
	void RegisterWindow();

private:
	// instance, window �ڵ�
	HINSTANCE		mInstanceHandle{ };
	WindowInfo		mWindowInfo{ };

	bool mKeyboardFocused{ false };
	bool mMouseCaptured{ false };

	// ������ ������ �̸�, Ŭ���� �̸� ����
	std::wstring	mWindowName{ L"GUI Ŭ���̾�Ʈ" };
	std::wstring	mWindowClassName{ L"GUI Ŭ���̾�Ʈ" };

	// Ÿ�̸�
	std::unique_ptr<class Timer> mTimer{ };

	// ������ ó��
	std::shared_ptr<class DrawBuffer> mDrawBuffer{ };

	// Ű�Է� ó��
	std::shared_ptr<class KeyInput> mKeyInput{ };

	// draw test
	std::unique_ptr<class Player> mPlayer{ };
	std::vector<std::unique_ptr<class Shape>> mShapes;
	std::vector<std::unique_ptr<class Object>> mObjects;

	/* ���� ���� */
	std::unordered_map<byte, std::unique_ptr<class Player>> mOtherPlayers{ };
	std::mutex mPlayerLock{ };
};