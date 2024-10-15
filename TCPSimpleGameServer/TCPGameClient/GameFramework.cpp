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

    // 연결에 실패하면 바로 종료
    bool connectSuccess = gServerService.ConnectToServer();
    if (not connectSuccess) {
        return false;
    }

    gServerService.CreateRecvThread();

    // 서버에 접속했다면 자신의 id를 부여받을 때까지 대기
    // 릴리즈모드에서의 컴파일러 최적화 때문에 volatile 함수로 설계
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

// 그리기에 필요한 객체들을 생성하는 함수
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
    // 플레이어 추가
    mOtherPlayers.emplace(id, std::make_unique<Player>(pos));
}

void GameFramework::UpdateJoinedPlayer(const byte id, const Position pos)
{
    if (id == gServerService.GetId()) {
        mPlayer->SetPosition(pos);
        return;
    }

    std::lock_guard playerGuard{ mPlayerLock };
    // 해당 플레이어 정보 업데이트
    if (mOtherPlayers.contains(id)) {
        mOtherPlayers[id]->SetPosition(pos);
    }
}

void GameFramework::ExitPlayer(const byte id)
{
    std::lock_guard playerGuard{ mPlayerLock };
    // Exit 패킷이 오는 경우 해당 id 플레이어가 존재하는지 확인 후 지운다.
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

// 마우스 메시지 처리 함수
void GameFramework::OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_LBUTTONUP:
        // 왼쪽 버튼을 누르고 떼면 점 생성 + 점 위치 서버로 보내기, 점위치 또한 카메라 위치를 기준으로 계산
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

// 업데이트 및 렌더링 함수
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
    mDrawBuffer->DrawString("지연률"s + std::to_string(mRecvTimeLatency) + "ms"s, 10, 80);
    mDrawBuffer->DrawString("다른 클라이언트: "s + std::to_string(mOtherPlayers.size()), 10, 110);
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
        ::MessageBox(nullptr, TEXT("윈도우 생성 실패"), TEXT("[에러] 윈도우 생성 실패"), MB_OK);
        ::exit(EXIT_FAILURE);
    }

    ShowWindow(mWindowInfo.windowHandle, SW_SHOW);
    UpdateWindow(mWindowInfo.windowHandle);

    // 윈도우 창이 만들어지면 클라이언트 영역을 가져오고, 키보드 포커스 설정
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
    // 메뉴가 필요하면 MAKEINTERSOURCEW 사용
    wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_TCPCLIENTWITHGUI);
    wcex.lpszClassName = mWindowClassName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);
}
