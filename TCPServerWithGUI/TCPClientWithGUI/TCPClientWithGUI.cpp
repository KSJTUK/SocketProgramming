// TCPClientWithGUI.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "resource.h"
#include "GameFramework.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (false == gGameFramework.Init(hInstance)) {
        ::PostQuitMessage(0);
        return EXIT_FAILURE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TCPCLIENTWITHGUI));

    MSG msg;
    // 기본 메시지 루프입니다:
    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message) {
                break;
            }

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
        else {
            gGameFramework.FrameAdvance();
        }
    }

    gGameFramework.Destroy();

    return (int) msg.wParam;
}