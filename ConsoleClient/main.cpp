// 클라이언트용 프로젝트
// socket 함수에 대한 내용은 Base 프로젝트에서 주석으로 설명.
// 여기에서는 Base 프로젝트에서 사용되지 않았던 함수들만 설명할 예정.
#include <iostream>

#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

constexpr unsigned short PORT_NUMBER = 7777;
constexpr unsigned short RECV_SIZE = 1024;

// 소켓 오류 코드를 확인하고 그에 맞는 오류내용을 출력
void HandleSocketError(const char* cause = nullptr)
{
    int socketErrorCode = ::WSAGetLastError();

    wchar_t errorMessageBuf[BUFSIZ]{ };
    ::FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        socketErrorCode,
        MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
        errorMessageBuf,
        BUFSIZ,
        nullptr
    );

    if (cause) {
        std::cout << cause << "\n";
    }

    std::cout << "Last Error Code: " << socketErrorCode << "\n";
    std::wcout << L"Formatted Error Message:" << errorMessageBuf << "\n";
}

int main()
{
    WSAData wsaData;
    if (0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        HandleSocketError();
        return EXIT_FAILURE;
    }

    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket) {
        // 에러 핸들링 추가
        return EXIT_FAILURE;
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = ::htons(PORT_NUMBER);
    // address.sin_addr.s_addr = ::inet_addr("127.0.0.1"); inet_addr은 구버전 함수이기에 컴파일러에서 경고발생
    int ptonResult = ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr.s_addr); // ip 주소
    if (0 == ptonResult) {
        std::cout << "IP 주소 형식이 지정한 IP 버전과 맞지 않음\n";
        return EXIT_FAILURE;
    }
    else if (-1 == ptonResult) {
        HandleSocketError();
        return EXIT_FAILURE;
    }

    // 해당 ip주소, 포트번호를 이용하여 실제로 서버와 연결하는 부분
    if (SOCKET_ERROR == ::connect(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in))) {
        HandleSocketError();
        return EXIT_FAILURE;
    }

    // 테스트용 recv
    // 서버에서 한번 send하므로 여기서도 한번 recv 하도록 작성
    // recv함수는 send함수와는 다르게 받은 데이터의 길이(수신한 바이트)를 반환함.
    // 0 -> 연결이 종료됨, SOCKET_ERROR -> 에러
    char recvData[RECV_SIZE]{ };
    int len = ::recv(socket, recvData, RECV_SIZE, 0);
    if (0 == len) {
        std::cout << "연결이 종료됨.\n";
        ::closesocket(socket);
        ::WSACleanup();
    }
    else if (SOCKET_ERROR == len) {
        HandleSocketError();
        return EXIT_FAILURE;
    }

    std::cout << "수신 바이트 수: " << len << "\n";
    std::cout << "내용: " << recvData << "\n";

    ::closesocket(socket);
    ::WSACleanup();

    // 시작 프로젝트를 여러개로 지정했을 때 바로 끝나지 않게 하기위해 작성
    char ch{ };
    std::cout << "끝내려면 엔터키를 눌러주세요.";
    ch = ::getchar();
}