#include "pch.h"
#include "GameFramework.h"
#include "Resource.h"
#include "DrawBuffer.h"
#include "Shape.h"

/* ----------------------------------------
* 
*				GameFramework
* 
  ---------------------------------------- */

GameFramework::GameFramework() = default;

GameFramework::~GameFramework() = default;

bool GameFramework::Init(HINSTANCE instanceHandle)
{
	mInstanceHandle = instanceHandle;

	RegisterWindow();
	CreateMyWindow();

    CreateObjects();

	return false;
}

void GameFramework::Destroy()
{
    // TODO
}

// 그리기에 필요한 객체들을 생성하는 함수
void GameFramework::CreateObjects()
{
    mDrawBuffer = std::make_shared<DrawBuffer>(mWindowInfo);

    mDrawTestShapes.emplace_back(std::make_unique<Square>(0, 0, 50, 50, mDrawBuffer ));
    for (int i = 0; i < 1000; ++i) {
        int x = Random::GetUniformRandom<int>(0, 1000);
        int y = Random::GetUniformRandom<int>(0, 800);
        mDrawTestShapes.emplace_back(std::make_unique<PointShape>(x, y, mDrawBuffer));
    }
}

LRESULT GameFramework::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND:
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return LRESULT{ 0 };
}

void GameFramework::FrameAdvance()
{
    mDrawBuffer->CleanupBuffer();

    // Rendering
    for (auto& shape : mDrawTestShapes) {
        shape->Render();
    }

    mDrawBuffer->CopyBufferMemToMain();
}

void GameFramework::CreateMyWindow()
{
    mWindowInfo.windowHandle = CreateWindowW(
        mWindowClassName.c_str(),
        mWindowName.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        nullptr,
        nullptr,
        mInstanceHandle,
        nullptr);

    if (!mWindowInfo.windowHandle) {
        ::MessageBox(nullptr, TEXT("윈도우 생성 실패"), TEXT("[에러] 윈도우 생성 실패"), MB_OK);
        ::exit(EXIT_FAILURE);
    }

    ShowWindow(mWindowInfo.windowHandle, SW_SHOW);
    UpdateWindow(mWindowInfo.windowHandle);

    GetClientRect(mWindowInfo.windowHandle, &mWindowInfo.windowRect);
}

void GameFramework::RegisterWindow()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &GameFramework::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = mInstanceHandle;
    wcex.hIcon = LoadIcon(mInstanceHandle, MAKEINTRESOURCE(IDI_TCPCLIENTWITHGUI));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    // 메뉴가 필요하면 MAKEINTERSOURCEW 사용
    wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_TCPCLIENTWITHGUI);
    wcex.lpszClassName = mWindowClassName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);
}
