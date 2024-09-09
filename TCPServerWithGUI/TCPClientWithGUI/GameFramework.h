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
	GameFramework();
	~GameFramework();

public:
	bool Init(HINSTANCE instanceHandle);
	void Destroy();

	// �׸��⿡ �ʿ��� ��ü���� �����ϴ� �Լ�
	void CreateObjects();
	void AddShape(class Shape* shape);

	// �ٸ� �÷��̾� ����, ����, ������Ʈ ó��
	void JoinOtherPlayer(byte id, class Player* player);
	void UpdateJoinedPlayer(byte id, Direction2D dir, float velocity);
	void ExitPlayer(byte id);

	// Getter
	HWND GetWindowHandle() const { return mWindowInfo.windowHandle; }
	std::shared_ptr<class KeyInput> GetKeyInput() const;
	std::shared_ptr<class DrawBuffer> GetDrawBuffer() const;

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

	// ������ ������ �̸�, Ŭ���� �̸� ����
	std::wstring	mWindowName{ L"GUI Ŭ���̾�Ʈ" };
	std::wstring	mWindowClassName{ L"GUI Ŭ���̾�Ʈ" };

	// ������ ó��
	std::shared_ptr<class DrawBuffer> mDrawBuffer{ };

	// Ű�Է� ó��
	std::shared_ptr<class KeyInput> mKeyInput{ };

	// ���� ������ �ۼ��� ó��
	std::unique_ptr<class ServerService> mServerService{ };

	// draw test
	std::unique_ptr<class Player> mPlayer{ };
	std::unordered_map<byte, std::unique_ptr<class Player>> mOtherPlayers{ };
	std::list<std::unique_ptr<class Shape>> mDrawTestShapes{ };
};