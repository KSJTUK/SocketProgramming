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

    // 서버에 접속했다면 자신의 id를 부여받을 때까지 대기
    // 릴리즈모드에서의 컴파일러 최적화 때문에 따로 volatile 변수를 만들어 검사
    volatile byte issuedId = mServerService->GetId();
    while (issuedId == NONE_CLIENT_ID) {
        issuedId = mServerService->GetId();
    }

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
    // 플레이어 추가
    mOtherPlayers.emplace(id, player);
}

void GameFramework::UpdateJoinedPlayer(byte id, Direction2D dir, float velocity)
{
    // 해당 플레이어 정보 업데이트
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

// 마우스 메시지 처리 함수
void GameFramework::OnProcessingMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_LBUTTONUP:
        // 왼쪽 버튼을 누르고 떼면 점 생성 + 점 위치 서버로 보내기
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
}

// 업데이트 및 렌더링 함수
void GameFramework::Update()
{
    mKeyInput->Input();
    mPlayer->Update();
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
        1920,
        1080,
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
