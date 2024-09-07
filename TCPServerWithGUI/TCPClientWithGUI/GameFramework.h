#pragma once

/* ----------------------------------------
*				GameFramework
* �⺻���� ������ ����, ������, ������Ʈ �Լ���
* System Call, ���� �������� ����� Ŭ����
* �� Ŭ������ ��ü�� Ư���� ������ ���ٸ� 
* �� �ϳ��� ���� ��
  ---------------------------------------- */

class GameFramework {
public:
	bool Init(HINSTANCE instanceHandle);
	void Destroy();

	// ������ �޼��� ó�� �Լ� 
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
};