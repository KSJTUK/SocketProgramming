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

	std::shared_ptr<class DrawBuffer> GetDrawBuffer() const;

	void OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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

	std::shared_ptr<class DrawBuffer> mDrawBuffer{ };
	std::unique_ptr<class ServerService> mServerService{ };

	// draw test
	std::list<std::unique_ptr<class Shape>> mDrawTestShapes{ };
};