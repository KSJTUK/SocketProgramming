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

    bool connectResult = mServerService->ConnectToServer();

    mServerService->CreateRecvThread();

    // 
    while (mServerService->GetId() == 0xFF) {

    }

    auto [playerX, playerY] = mPlayer->GetPosition();
    PacketPlayerJoin joinedPacket{ sizeof(PacketPlayerJoin), PACKET_PLAYER_JOIN, mServerService->GetId(), playerX, playerY };
    mServerService->Send(&joinedPacket);

	return true;
}

void GameFramework::Destroy()
{
    // TODO
    mServerService.reset();
    mDrawBuffer.reset();
    mDrawTestShapes.clear();
}

// 그리기에 필요한 객체들을 생성하는 함수
void GameFramework::CreateObjects()
{
    mKeyInput = std::make_shared<KeyInput>();
    mServerService = std::make_unique<ServerService>();
    mDrawBuffer = std::make_shared<DrawBuffer>(mWindowInfo);

    mPlayer = std::make_unique<Player>();
}

void GameFramework::AddShape(Shape* shape)
{
    mDrawTestShapes.emplace_back(shape);
}

void GameFramework::JoinOtherPlayer(byte id, Player* player)
{
    mOtherPlayers.try_emplace(id, player);
}

void GameFramework::UpdateJoinedPlayer(byte id, Direction2D dir, float velocity)
{
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

std::shared_ptr<class KeyInput> GameFramework::GetKeyInput() const
{
    return mKeyInput;
}

std::shared_ptr<class DrawBuffer> GameFramework::GetDrawBuffer() const
{
    return mDrawBuffer;
}

void GameFramework::OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_LBUTTONUP:
        ReleaseCapture();
        {
            PacketPosition2D* positionPacket = new PacketPosition2D;
            positionPacket->size = sizeof(PacketPosition2D);
            positionPacket->type = PACKET_POSITION2D;
            positionPacket->x = LOWORD(lParam);
            positionPacket->y = HIWORD(lParam);
            mServerService->Send(positionPacket);
        }
        break;

    case WM_LBUTTONDOWN:
        SetCapture(mWindowInfo.windowHandle);
        break;
    }
}

void GameFramework::OnProcessingKeyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}

LRESULT GameFramework::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_KEYDOWN:
        gGameFramework.OnProcessingKeyboard(hWnd, message, wParam, lParam);
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        gGameFramework.OnProcessingMouse(hWnd, message, wParam, lParam);
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

void GameFramework::FrameAdvance()
{
    mKeyInput->Input();
    mDrawBuffer->CleanupBuffer();

    mPlayer->Update();
    auto [dirX, dirY] = mPlayer->GetDirection();
    PacketMove2D playerPacket{ sizeof(PacketMove2D), PACKET_MOVE2D, mServerService->GetId(), dirX, dirY, mPlayer->GetVelocity()};
    mServerService->Send(&playerPacket);

    for (auto& [id, otherPlayer] : mOtherPlayers) {
        otherPlayer->Update();
    }

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
