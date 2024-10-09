#include "pch.h"
#include "GameFramework.h"
#include "Resource.h"
#include "ServerService.h"

#include "Timer.h"
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

    // 연결에 실패하면 바로 종료
    bool connectSuccess = mServerService->ConnectToServer();
    if (not connectSuccess) {
        return false;
    }

    mServerService->CreateRecvThread();

    // 서버에 접속했다면 자신의 id를 부여받을 때까지 대기
    // 릴리즈모드에서의 컴파일러 최적화 때문에 volatile 함수로 설계
    while (NULL_CLIENT_ID == mServerService->GetId());

    // 최초 접속시 자신의 위치를 다른 클라이언트에게 알리기위해 서버에 전송
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

    mTimer.reset();
    mServerService.reset();
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
    mServerService = std::make_unique<ServerService>();
    mPlayer = std::make_unique<Player>(true);

    // 플레이어의 키 입력처리 함수들을 등록
    mPlayer->RegisterExecutionFn();

    // 플레이어의 시작 위치를 랜덤하게 지정
    auto [minX, minY, maxX, maxY] = mWindowInfo.windowRect;
    mPlayer->SetPosition(
        static_cast<float>(Random::GetUniformRandom(minX, maxX)),
        static_cast<float>(Random::GetUniformRandom(minY, maxY))
    );

    CreatePointsFromFile();
}

void GameFramework::JoinOtherPlayer(byte id, float x, float y)
{
    std::lock_guard playerGuard{ mPlayerLock };
    // 플레이어 추가
    mOtherPlayers.emplace(id, std::make_unique<Player>(x, y));
}

void GameFramework::UpdateJoinedPlayer(byte id, Position pos)
{
    std::lock_guard playerGuard{ mPlayerLock };
    // 해당 플레이어 정보 업데이트
    if (mOtherPlayers.contains(id)) {
        mOtherPlayers[id]->SetPosition(pos);
    }
}

void GameFramework::ExitPlayer(byte id)
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

void GameFramework::CreatePointsFromFile()
{
    std::ifstream in{ "Entities.bin", std::ios::binary };
    if (not in) {
        return;
    }

    size_t numOfPoints{ };
    in.read(reinterpret_cast<char*>(&numOfPoints), sizeof(size_t));

    std::vector<Position> positions(numOfPoints);
    in.read(reinterpret_cast<char*>(positions.data()), numOfPoints * sizeof(Position));

    mShapes.reserve(numOfPoints);
    for (const auto& position : positions) {
        mShapes.emplace_back(std::make_unique<Square>(position, 5, 5, mDrawBuffer));
    }
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

// 업데이트 및 렌더링 함수
void GameFramework::Update()
{
    mTimer->Update();
    const float deltaTime = mTimer->GetDeltaTime();

    // 현재 프로세스가 아닌 다른 프로세스에서의 입력은 감지하지 않도록 설정
    if (mKeyboardFocused) {
        mKeyInput->Input(deltaTime);
    }

    mServerService->Send<PacketPing>(PACKET_PING, mTimer->GetCurrentTick());

    mPlayer->Update(deltaTime);
    mDrawBuffer->SetCameraPosition(mPlayer->GetPosition());

    auto [playerX, playerY] = mPlayer->GetPosition();
    mServerService->Send<PacketPosition2D>(PACKET_POSITION2D, playerX, playerY);

    for (auto& [id, otherPlayer] : mOtherPlayers) {
        otherPlayer->Update(deltaTime);
    }
}

void GameFramework::Render()
{
    mDrawBuffer->CleanupBuffer();

    gGameFramework.GetDrawBuffer()->DrawString(std::to_string(mTimer->GetFPS()), 10, 50);

    for (auto& [id, otherPlayer] : mOtherPlayers) {
        otherPlayer->Render();
    }

    for (auto& shape : mShapes) {
        shape->Render();
    }

    mPlayer->Render();

    mDrawBuffer->DrawString("FPS: "s + std::to_string(mTimer->GetFPS()), 10, 20);
    mDrawBuffer->DrawString("Delta Time: "s + std::to_string(mTimer->GetDeltaTime()) + "s"s, 10, 50);
    mDrawBuffer->DrawString("지연률"s + std::to_string(mRecvTimeLatency) + "ms"s, 10, 80);
    mDrawBuffer->DrawString("다른 클라이언트: "s + std::to_string(mOtherPlayers.size()), 10, 110);

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
