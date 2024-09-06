﻿#include <iostream>
#include <thread>

#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

constexpr unsigned short PORT = 7777;
constexpr int RECV_SIZE = 1024;
constexpr int SEND_SIZE = 1024;

// 소켓 함수관련 에러 발생시 사용할 함수들
// 에러 메시지 출력
void HandleErrorDisplay()
{
    int errorCode = ::WSAGetLastError();

    void* errorMessage = nullptr;
    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<char*>(&errorMessage),
        0,
        nullptr
    );

    std::cout << "[소켓 에러 발생] 에러 코드: " << errorCode << "\n";
    std::cout << "[내용]: " << errorMessage;

    LocalFree(errorMessage);
}

// 에러 메시지 출력 후 프로그램 강제 종료
void HandleErrorQuit()
{
    HandleErrorDisplay();
    ::exit(EXIT_FAILURE);

    char ch;
    std::cout << "끝내려면 엔터키를 눌러주세요\n";
    ch = getchar();
}

// 데이터 송신 함수
void Send(SOCKET socket, const char* data, size_t dataSize)
{
    if (SOCKET_ERROR == ::send(socket, data, dataSize, 0)) {
        HandleErrorQuit();
    }
}

// 데이터 수신 함수
void Recv(SOCKET socket)
{
    char buffer[RECV_SIZE]{ };

    while (true) {
        std::memset(buffer, 0, RECV_SIZE);

        int len = ::recv(socket, buffer, RECV_SIZE, 0);
        if (len == 0) {
            std::cout << "연결 종료\n";
            ::shutdown(socket, SD_BOTH);
            break;
        }
        else if (len < 0) {
            HandleErrorQuit();
        }

        std::cout << "----------------------------------------\n";
        std::cout << "수신: " << buffer << "\n";
        std::cout << "----------------------------------------\n\n";
    }
}

// 윈도우 소켓 초기화(ws2_32.dll)
void WinSockStart()
{
    WSAData wsaData;
    if (0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        HandleErrorQuit();
    }
}

// 윈도우 소켓 사용 종료(ws2_32.dll)
void WinSockClean()
{
    if (0 != ::WSACleanup()) {
        HandleErrorQuit();
    }
}

void TCPClientIPv4()
{
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket) {
        HandleErrorQuit();
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = ::htons(PORT);
    int ptonResult = ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr.s_addr); // ip 주소
    if (0 == ptonResult) {
        std::cout << "IP 주소 형식이 지정한 IP 버전과 맞지 않음\n";
        ::exit(EXIT_FAILURE);
    }
    else if (-1 == ptonResult) {
        HandleErrorQuit();
        ::exit(EXIT_FAILURE);
    }

    if (SOCKET_ERROR == ::connect(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in))) {
        HandleErrorQuit();
    }

    // 데이터 수신 쓰레드 생성
    std::thread recvThread{ Recv, socket };

    std::string sendData{ };
    // 데이터 송신 루프
    while (true) {
        // 보낼 문자열 입력 받기 및, 입력제한 조건 설정
        std::cout << "보낼 문자열을 입력해주세요 (끝내려면 quit 입력, 입력 제한 1023자): ";
        std::cin >> sendData;

        size_t dataSize = sendData.size();
        if (SEND_SIZE <= dataSize) {
            std::cout << "입력제한 1023자를 넘어섰습니다. 다시 입력해주세요.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<int>::max(), '\n');
            continue;
        }

        // 끝나는지?
        if (0 == std::strcmp(sendData.c_str(), "quit")) {
            ::shutdown(socket, SD_BOTH);
            break;
        }

        // 문자열 보내기
        if (SOCKET_ERROR == ::send(socket, sendData.c_str(), dataSize, 0)) {
            HandleErrorQuit();
        }
        std::cout << "보내기 완료.\n\n";
        std::cout << "----------------------------------------\n\n";
    }

    // 끝날때 까지 기다려주기
    recvThread.join();
}

int main()
{
    WinSockStart();

    TCPClientIPv4();

    WinSockClean();

    // 동시에 여러 프로젝트를 실행할 때 바로 끝나는 걸 방지하기 위한 코드
    char ch;
    std::cout << "끝내려면 엔터키를 눌러주세요\n";
    ch = getchar();
}