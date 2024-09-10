#include "pch.h"
#include "GameFramework.h"
#include "Resource.h"
#include "ServerService.h"

#include "Input.h"
#include "DrawBuffer.h"
#include "Shape.h"
#include "Player.h"

/* ----------------------------------------
*
*			  Window Procedure
*
  ---------------------------------------- */   

LRESULT GameFramework::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_KEYDOWN:
    case WM_KEYUP:
        gGameFramework.OnProcessingKeyboard(hWnd, message, wParam, lParam);
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        gGameFramework.OnProcessingMouse(hWnd, message, wParam, lParam);
        break;

    case WM_SETFOCUS:
        gGameFramework.SetKeyboardFocuse(true);
        break;

    case WM_KILLFOCUS:
        gGameFramework.SetKeyboardFocuse(false);
        break;

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

    // ���ῡ �����ϸ� �ٷ� ����
    bool connectSuccess = mServerService->ConnectToServer();
    if (not connectSuccess) {
        return false;
    }

    mServerService->CreateRecvThread();

    // ������ �����ߴٸ� �ڽ��� id�� �ο����� ������ ���
    // �������忡���� �����Ϸ� ����ȭ ������ ���� volatile ������ ����� �˻�
    volatile byte issuedId = mServerService->GetId();
    while (issuedId == NONE_CLIENT_ID) {
        issuedId = mServerService->GetId();
    }

    // ���� ���ӽ� �ڽ��� ��ġ�� �ٸ� Ŭ���̾�Ʈ���� �˸������� ������ ����
    auto [playerX, playerY] = mPlayer->GetPosition();
    mServerService->Send<PacketPlayerJoin>(PACKET_PLAYER_JOIN, playerX, playerY);

	return true;
}

void GameFramework::Destroy()
{
    mServerService->Send<PacketPlayerExit>(PACKET_PLAYER_EXIT);

    // TODO
    mServerService->Join();

    mServerService.reset();
    mDrawBuffer.reset();
    mDrawTestShapes.clear();
}

void GameFramework::SetKeyboardFocuse(bool focused)
{
    mKeyboardFocused = focused;
}

void GameFramework::SetMouseCapture(bool captured)
{
    mMouseCaptured = captured;
}

// �׸��⿡ �ʿ��� ��ü���� �����ϴ� �Լ�
void GameFramework::CreateObjects()
{
    mKeyInput = std::make_shared<KeyInput>();
    mServerService = std::make_unique<ServerService>();
    mDrawBuffer = std::make_shared<DrawBuffer>(mWindowInfo);

    mPlayer = std::make_unique<Player>(true);
    auto [minX, minY, maxX, maxY] = mWindowInfo.windowRect;
    mPlayer->SetPosition(
        static_cast<float>(Random::GetUniformRandom(minX, maxX)),
        static_cast<float>(Random::GetUniformRandom(minY, maxY))
    );
}

void GameFramework::AddShape(Shape* shape)
{
    mDrawTestShapes.emplace_back(shape);
}

void GameFramework::JoinOtherPlayer(byte id, Player* player)
{
    // �÷��̾� �߰�
    mOtherPlayers.emplace(id, player);
}

void GameFramework::UpdateJoinedPlayer(byte id, Direction2D dir, float velocity)
{
    // �ش� �÷��̾� ���� ������Ʈ
    if (mOtherPlayers.contains(id)) {
        mOtherPlayers[id]->SetDirection(dir);
        mOtherPlayers[id]->SetValocity(velocity);
    }
}

void GameFramework::ExitPlayer(byte id)
{
    if (mOtherPlayers.contains(id)) {
        mOtherPlayers.erase(id);
    }
}

std::shared_ptr<KeyInput> GameFramework::GetKeyInput() const
{
    return mKeyInput;
}

std::shared_ptr<DrawBuffer> GameFramework::GetDrawBuffer() const
{
    return mDrawBuffer;
}

// ���콺 �޽��� ó�� �Լ�
void GameFramework::OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_LBUTTONUP:
        // ���� ��ư�� ������ ���� �� ���� + �� ��ġ ������ ������
        ReleaseCapture();
        {
            float x = static_cast<float>(LOWORD(lParam));
            float y = static_cast<float>(HIWORD(lParam));
            mServerService->Send<PacketPosition2D>(PACKET_POSITION2D, x, y);
            AddShape(new PointShape{ x, y, mDrawBuffer});
        }
        break;

    case WM_LBUTTONDOWN:
        SetCapture(mWindowInfo.windowHandle);
        break;
    }
}

void GameFramework::OnProcessingKeyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_KEYUP:
        switch (wParam) {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

// ������Ʈ �� ������ �Լ�
void GameFramework::Update()
{
    // ���� ���μ����� �ƴ� �ٸ� ���μ��������� �Է��� �������� �ʵ��� ����
    if (mKeyboardFocused) {
        mKeyInput->Input();
    }

    mPlayer->Update();
    mDrawBuffer->SetCameraPosition(mPlayer->GetPosition());

    auto [dirX, dirY] = mPlayer->GetDirection();
    mServerService->Send<PacketMove2D>(PACKET_MOVE2D, dirX, dirY, mPlayer->GetVelocity());

    for (auto& [id, otherPlayer] : mOtherPlayers) {
        otherPlayer->Update();
    }
}

void GameFramework::Render()
{
    mDrawBuffer->CleanupBuffer();

    // Rendering
    for (auto& shape : mDrawTestShapes) {
        shape->Render();
    }

    for (auto& [id, otherPlayer] : mOtherPlayers) {
        otherPlayer->Render();
    }

    mPlayer->Render();

    mDrawBuffer->CopyBufferMemToMain();
}

void GameFramework::FrameAdvance()
{
    Update();
    Render();
}

void GameFramework::CreateMyWindow()
{
    mWindowInfo.windowHandle = CreateWindowW(
        mWindowClassName.c_str(),
        mWindowName.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        1280,
        720,
        nullptr,
        nullptr,
        mInstanceHandle,
        nullptr);

    if (!mWindowInfo.windowHandle) {
        ::MessageBox(nullptr, TEXT("������ ���� ����"), TEXT("[����] ������ ���� ����"), MB_OK);
        ::exit(EXIT_FAILURE);
    }

    ShowWindow(mWindowInfo.windowHandle, SW_SHOW);
    UpdateWindow(mWindowInfo.windowHandle);

    // ������ â�� ��������� Ŭ���̾�Ʈ ������ ��������, Ű���� ��Ŀ�� ����
    GetClientRect(mWindowInfo.windowHandle, &mWindowInfo.windowRect);
    SetFocus(mWindowInfo.windowHandle);
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
    // �޴��� �ʿ��ϸ� MAKEINTERSOURCEW ���
    wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_TCPCLIENTWITHGUI);
    wcex.lpszClassName = mWindowClassName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);
}
