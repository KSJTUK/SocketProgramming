// 클라이언트용 프로젝트
// socket 함수에 대한 내용은 Base 프로젝트에서 주석으로 설명.
// 여기에서는 Base 프로젝트에서 사용되지 않았던 함수들만 설명할 예정.
#include <iostream>

#define NOMINMAX

#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

constexpr unsigned short PORT_NUMBER = 7777;
constexpr unsigned short SEND_SIZE = 1024;
constexpr unsigned short RECV_SIZE = 1024;

// 소켓 오류 코드를 확인하고 그에 맞는 오류내용을 출력
void HandleSocketError(const char* cause = nullptr)
{
    int socketErrorCode = ::WSAGetLastError();

    void* errorMessage = nullptr;
    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        nullptr,
        socketErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<char*>(&errorMessage),
        0,
        nullptr
    );

    if (cause) {
        std::cout << cause << "\n";
    }

    std::cout << "Last Error Code: " << socketErrorCode << "\n";
    std::cout << "Formatted Error Message:" << reinterpret_cast<char*>(errorMessage) << "\n";

    LocalFree(errorMessage);
}

// 중복될 수 있는 WSAStartup, WSACleanup은 따로 함수를 만들어 관리
void WinSockStart()
{
    // WSAStartup 윈도우 소켓 초기화를 위한 변수, 사용되는 일은 적음
    WSAData wsaData;

    if (0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        HandleSocketError();
        exit(EXIT_FAILURE);
    }
}

void WinSockClean()
{
    // ws2_32.dll의 사용을 종료하는 함수
    // MSDN에서는 WSAStartup과 쌍을 이루어 호출하는 것을 권장하고 있음.
    // 자세한 설명은 아래 링크 참고
    // https://learn.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsacleanup
    if (0 != ::WSACleanup()) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }
}

// 
void RecvAndSend(SOCKET socket)
{
    std::string sendData{ };
    char recvData[RECV_SIZE]{ };
    while (true) {
        sendData.clear();
        std::memset(recvData, 0, SEND_SIZE);

        // 보냈으면 받기
        // recv함수는 send함수와는 다르게 받은 데이터의 길이(수신한 바이트)를 반환함.
        // 0 -> 연결이 종료됨, SOCKET_ERROR -> 에러
        std::cout << "----------------------------------------\n\n";
        std::cout << "수신 대기...\n\n";
        int len = ::recv(socket, recvData, RECV_SIZE, 0);
        if (0 == len) {
            std::cout << "연결 종료.\n";
            break;
        }
        else if (SOCKET_ERROR == len) {
            HandleSocketError();
            break;
        }
        else if ('\0' != recvData[RECV_SIZE - 1]) {
            std::cout << "recv 한도 초과\n";
            break;
        }

        std::cout << "수신 바이트 수: " << len << "\n";
        std::cout << "내용: " << recvData << "\n\n";
        std::cout << "----------------------------------------\n\n";

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
            break;
        }

        // 문자열 보내기
        if (SOCKET_ERROR == ::send(socket, sendData.c_str(), dataSize, 0)) {
            HandleSocketError();
            ::exit(EXIT_FAILURE);
        }

        std::cout << "보내기 완료.\n\n";
        std::cout << "----------------------------------------\n\n";

        // 다 받으면 다시 반복
    }
}

// IPv4를 사용하는 TCP 클라이언트
void TCPClientIPv4()
{
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket) {
        // 에러 핸들링 추가
        ::exit(EXIT_FAILURE);
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = ::htons(PORT_NUMBER);
    // address.sin_addr.s_addr = ::inet_addr("127.0.0.1"); inet_addr은 구버전 함수이기에 컴파일러에서 경고발생
    int ptonResult = ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr.s_addr); // ip 주소
    if (0 == ptonResult) {
        std::cout << "IP 주소 형식이 지정한 IP 버전과 맞지 않음\n";
        ::exit(EXIT_FAILURE);
    }
    else if (-1 == ptonResult) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    // 해당 ip주소, 포트번호를 이용하여 실제로 서버와 연결하는 부분
    if (SOCKET_ERROR == ::connect(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in))) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    std::cout << "연결 성공!\n";

    RecvAndSend(socket);

    ::closesocket(socket);
}

// IPv6를 사용하는 TCP 클라이언트
void TCPClientIPv6()
{
    SOCKET socket = ::socket(AF_INET6, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket) {
        // 에러 핸들링 추가
        ::exit(EXIT_FAILURE);
    }

    sockaddr_in6 address;
    std::memset(&address, 0, sizeof(sockaddr_in6));
    address.sin6_family = AF_INET6;
    address.sin6_port = ::htons(PORT_NUMBER);
    // address.sin_addr.s_addr = ::inet_addr("127.0.0.1"); inet_addr은 구버전 함수이기에 컴파일러에서 경고발생
    int ptonResult = ::inet_pton(AF_INET6, "0:0:0:0:0:0:0:1", &address.sin6_addr); // ip 주소
    if (0 == ptonResult) {
        std::cout << "IP 주소 형식이 지정한 IP 버전과 맞지 않음\n";
        ::exit(EXIT_FAILURE);
    }
    else if (-1 == ptonResult) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    // 해당 ip주소, 포트번호를 이용하여 실제로 서버와 연결하는 부분
    if (SOCKET_ERROR == ::connect(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in6))) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    std::cout << "연결 성공!\n";

    RecvAndSend(socket);

    ::closesocket(socket);
}

int main()
{
    WinSockStart();

    TCPClientIPv6();

    WinSockClean();

    // 시작 프로젝트를 여러개로 지정했을 때 바로 끝나지 않게 하기위해 작성
    char ch{ };
    std::cout << "끝내려면 엔터키를 눌러주세요.";
    ch = ::getchar();
}