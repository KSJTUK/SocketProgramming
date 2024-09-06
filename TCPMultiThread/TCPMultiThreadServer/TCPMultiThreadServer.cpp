#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <mutex>

#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <winioctl.h>

#pragma comment(lib, "ws2_32.lib")

constexpr unsigned short PORT = 7777;
constexpr int RECV_SIZE = 1024;
constexpr int SEND_SIZE = 1024;

// 콘솔 제어 함수들
// 콘솔 너비를 반환하는 함수
int GetTerminalWidth(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    return columns;
}

// 캐리지 리턴을 이용해서 한라인만 공백으로 채우는 함수
void EraseConsoleLine()
{
    std::cout << "\r" << std::string(GetTerminalWidth(), ' ') << "\r";
}

void ClearConsole()
{
    ::system("cls");
}

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
    std::cout << "[내용]: " << reinterpret_cast<char*>(errorMessage);

    LocalFree(errorMessage);
}

// 에러 메시지 출력 후 프로그램 강제 종료
void HandleErrorQuit()
{
    HandleErrorDisplay();
    ::exit(EXIT_FAILURE);
}

// 클라이언트 확인용, string으로 변환한 ip주소와 포트번호 반환
std::pair<std::string, unsigned short> NetAddr(sockaddr_in address)
{
    char ip[INET_ADDRSTRLEN]{ };
    ::inet_ntop(AF_INET, &address.sin_addr, ip, INET_ADDRSTRLEN);
    unsigned short port = ::ntohs(address.sin_port);

    return std::make_pair(ip, port);
}

std::pair<std::string, unsigned short> NetAddr(sockaddr_in6 address)
{
    char ip[INET6_ADDRSTRLEN]{ };
    ::inet_ntop(AF_INET6, &address.sin6_addr, ip, INET6_ADDRSTRLEN);
    unsigned short port = ::ntohs(address.sin6_port);

    return std::make_pair(ip, port);
}

// 데이터 수신 및 돌려주기 함수
void RecvAndEcho(SOCKET clientSocket, std::pair<std::string, unsigned short> address)
{
    auto [ip, port] = address;
    std::cout << "[클라이언트 접속] IP: " << ip << ", PORT 번호: " << port << "\n\n";

    char buffer[RECV_SIZE]{ };

    while (true) {
        std::memset(buffer, 0, RECV_SIZE);

        int len = ::recv(clientSocket, buffer, RECV_SIZE, 0);
        if (len == 0) {
            EraseConsoleLine();
            std::cout << "[클라이언트 연결 종료] IP: " << ip << ", PORT 번호: " << port << "\n\n";
            ::shutdown(clientSocket, SD_BOTH);
            break;
        }
        else if (len < 0) {
            HandleErrorDisplay();
            std::cout << "IP: " << ip << ", PORT 번호 : " << port << "\n\n";
            break;
        }

        EraseConsoleLine();

        std::cout << "[수신 " << len << "bytes]: " << buffer << "\n";

        if (SOCKET_ERROR == ::send(clientSocket, buffer, (int)(strlen(buffer)), 0)) {
            HandleErrorQuit();
        }
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

// IPv4 를 사용하는 TCP 서버
void TCPServerIPv4()
{
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listenSocket) {
        HandleErrorQuit();
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = ::htons(PORT);
    address.sin_addr.s_addr = ::htonl(INADDR_ANY);

    if (SOCKET_ERROR == ::bind(listenSocket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in))) {
        HandleErrorQuit();
    }

    if (SOCKET_ERROR == listen(listenSocket, SOMAXCONN)) {
        HandleErrorQuit();
    }

    std::vector<std::thread> serverThreads{ };

    sockaddr_in clientAddress;
    int addressLength{ sizeof(sockaddr_in) };
    while (true) {
        std::memset(&clientAddress, 0, sizeof(sockaddr_in));
        SOCKET clientSocket = ::accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddress), &addressLength);
        if (INVALID_SOCKET == clientSocket) {
            HandleErrorDisplay();
            continue;
        }
        
        serverThreads.emplace_back(RecvAndEcho, clientSocket, NetAddr(clientAddress));
    }

    for (auto& thread : serverThreads) {
        thread.join();
    }
}

// IPv6를 사용하는 서버
void TCPServerIPv6()
{
    SOCKET listenSocket = ::socket(AF_INET6, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listenSocket) {
        HandleErrorQuit();
    }

    sockaddr_in6 address;
    std::memset(&address, 0, sizeof(sockaddr_in6));
    address.sin6_family = AF_INET6;
    address.sin6_port = ::htons(PORT);
    address.sin6_addr = in6addr_any;

    if (SOCKET_ERROR == ::bind(listenSocket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in6))) {
        HandleErrorQuit();
    }

    if (SOCKET_ERROR == ::listen(listenSocket, SOMAXCONN)) {
        HandleErrorQuit();
    }

    std::vector<std::thread> serverThreads{ };

    sockaddr_in6 clientAddress;
    int clientAddressLen{ sizeof(sockaddr_in6) };
    while (true) {
        std::memset(&clientAddress, 0, sizeof(sockaddr_in));
        SOCKET clientSocket = ::accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLen);
        if (INVALID_SOCKET == clientSocket) {
            HandleErrorDisplay();
            continue;
        }

        serverThreads.emplace_back(RecvAndEcho, clientSocket, NetAddr(clientAddress));
    }

    for (auto& thread : serverThreads) {
        thread.join();
    }
}

int main()
{
    WinSockStart();

    TCPServerIPv6();

    WinSockClean();

    // 동시에 여러 프로젝트를 실행할 때 바로 종료되는 것을 방지하기 위한 코드
    char ch;
    std::cout << "종료하려면 엔터키를 눌러주세요.";
    ch = getchar();
}