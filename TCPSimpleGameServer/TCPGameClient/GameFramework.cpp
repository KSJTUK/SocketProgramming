#include "pch.h"
#include "GameFramework.h"
#include "Resource.h"
#include "ServerService.h"

#include "Timer.h"
#include "Input.h"
#include "DrawBuffer.h"
#include "Object.h"
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
    bool connectSuccess = gServerService.ConnectToServer();
    if (not connectSuccess) {
        return false;
    }

    gServerService.CreateRecvThread();

    // ������ �����ߴٸ� �ڽ��� id�� �ο����� ������ ���
    // �������忡���� �����Ϸ� ����ȭ ������ volatile �Լ��� ����
    while (NULL_CLIENT_ID == gServerService.GetId());

    gServerService.Send<PacketPlayerConnect>(PACKET_PLAYER_CONNECT);

    return true;
}

void GameFramework::Destroy()
{
    gServerService.Send<PacketPlayerExit>(PACKET_PLAYER_EXIT);

    // TODO
    gServerService.Join();

    mTimer.reset();
    mDrawBuffer.reset();
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
    // shared_ptr
    mKeyInput = std::make_shared<KeyInput>();
    mDrawBuffer = std::make_shared<DrawBuffer>(mWindowInfo, RGB(127, 127, 127));

    // unique_ptr
    mTimer = std::make_unique<Timer>();
    mPlayer = std::make_unique<Player>();

    mObjects.resize(MAX_OBJECT);
}

void GameFramework::JoinOtherPlayer(const byte id, const Position pos)
{
    std::lock_guard playerGuard{ mPlayerLock };
    // �÷��̾� �߰�
    mOtherPlayers.emplace(id, std::make_unique<Player>(pos));
}

void GameFramework::UpdateJoinedPlayer(const byte id, const Position pos)
{
    if (id == gServerService.GetId()) {
        mPlayer->SetPosition(pos);
        return;
    }

    std::lock_guard playerGuard{ mPlayerLock };
    // �ش� �÷��̾� ���� ������Ʈ
    if (mOtherPlayers.contains(id)) {
        mOtherPlayers[id]->SetPosition(pos);
    }
}

void GameFramework::ExitPlayer(const byte id)
{
    std::lock_guard playerGuard{ mPlayerLock };
    // Exit ��Ŷ�� ���� ��� �ش� id �÷��̾ �����ϴ��� Ȯ�� �� �����.
    if (mOtherPlayers.contains(id)) {
        mOtherPlayers.erase(id);
    }
}

void GameFramework::PingResult(std::chrono::high_resolution_clock::time_point timeSent)
{
    unsigned long long latency = std::chrono::duration_cast<std::chrono::milliseconds>(mTimer->GetCurrentTick() - timeSent).count() / 2;
    mRecvTimeLatency.store(latency);
}

std::shared_ptr<KeyInput> GameFramework::GetKeyInput() const
{
    return mKeyInput;
}

std::shared_ptr<DrawBuffer> GameFramework::GetDrawBuffer() const
{
    return mDrawBuffer;
}

void GameFramework::UpdateObject(PacketObjectInfo* objectInfo)
{
    //int idx = objectInfo->objectIndex;
    //if (not mObjects[idx]) {
    //    mObjects[idx].reset();
    //}
}

// ���콺 �޽��� ó�� �Լ�
void GameFramework::OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_LBUTTONUP:
        // ���� ��ư�� ������ ���� �� ���� + �� ��ġ ������ ������, ����ġ ���� ī�޶� ��ġ�� �������� ���
        ReleaseCapture();
        break;

    case WM_LBUTTONDOWN:
        SetCapture(mWindowInfo.windowHandle);
        break;
    }
}

void GameFramework::OnProcessingKeyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_KEYDOWN:
        gServerService.Send<PacketPlayerInput>(PACKET_PLAYER_INPUT, static_cast<byte>(wParam), true);
        break;

    case WM_KEYUP:
        gServerService.Send<PacketPlayerInput>(PACKET_PLAYER_INPUT, static_cast<byte>(wParam), false);

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
    mTimer->Update();
    const float deltaTime = mTimer->GetDeltaTime();

    ////gServerService.Send<PacketPing>(PACKET_PING, mTimer->GetCurrentTick());

    //for (auto& [id, otherPlayer] : mOtherPlayers) {
    //    otherPlayer->Update(deltaTime);
    //}

    //mPlayer->Update(deltaTime);

    mDrawBuffer->SetCameraPosition(mPlayer->GetPosition());
}

void GameFramework::Render()
{
    mDrawBuffer->CleanupBuffer();

    mDrawBuffer->DrawString(std::to_string(mTimer->GetFPS()), 10, 50);

    for (auto& [id, otherPlayer] : mOtherPlayers) {
        otherPlayer->Render();
    }

    //for (auto& object : mObjects) {
    //    if (object)
    //        object->Render(mDrawBuffer);
    //}

    mPlayer->Render();

    mDrawBuffer->DrawString("FPS: "s + std::to_string(mTimer->GetFPS()), 10, 20);
    mDrawBuffer->DrawString("Delta Time: "s + std::to_string(mTimer->GetDeltaTime()) + "s"s, 10, 50);
    mDrawBuffer->DrawString("������"s + std::to_string(mRecvTimeLatency) + "ms"s, 10, 80);
    mDrawBuffer->DrawString("�ٸ� Ŭ���̾�Ʈ: "s + std::to_string(mOtherPlayers.size()), 10, 110);
    mDrawBuffer->DrawString("My ID: "s + std::to_string(static_cast<int>(gServerService.GetId())), 10, 140);

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
        800,
        600,
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
    wcex.hIcon = LoadIcon(mInstanceHandle, MAKEINTRESOURCE(IDI_TCPGAMECLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    // �޴��� �ʿ��ϸ� MAKEINTERSOURCEW ���
    wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_TCPCLIENTWITHGUI);
    wcex.lpszClassName = mWindowClassName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);
}
