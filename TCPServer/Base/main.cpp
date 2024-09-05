#include <iostream>

// Windows의 매크로 함수 min, max 사용중지를 위한 코드
// std::min, max와의 충돌 가능성 때문에 사용
#define NOMINMAX

// 소켓 프로그래밍을 위한 헤더
#include <WinSock2.h>
#include <WS2tcpip.h>

// ws2_32 라이브러리, ws2_32.dll과 링크
#pragma comment(lib, "ws2_32.lib")

// STL 헤더
#include <numeric>

// 사용할 포트 번호 정의, 포트 번호는 16비트 정수를 이용 (unsigned short)
constexpr unsigned short PORT_NUMBER = 7777;
// 테스트용으로 보낼 문자열의 최대 길이 정의
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

// 두 IP버전 모두 같은 로직으로 Send, Recv 하므로 Socket만 인자로 받아 함수로 처리
void SendAndRecv(SOCKET clientSocket)
{
    // 테스트용 send 잘 가는지는 클라이언트 부분을 만들어서 확인!
    // 주의! 처음에 만든 소켓은 서버에서 클라이언트와 연결을 위한 소켓임, accept를 위해 생성한 소켓을 이용

    std::string sendData{ };
    char recvData[RECV_SIZE]{ };
    while (true) {
        sendData.clear();
        std::memset(recvData, 0, SEND_SIZE);

        std::cout << "----------------------------------------\n\n";
        // 보낼 문자열 입력 받기 및, 입력제한 조건 설정
        std::cout << "보낼 문자열을 입력해주세요 (끝내려면 quit 입력, 입력제한 1023자): ";
        std::cin >> sendData;

        size_t dataSize = sendData.size();
        if (SEND_SIZE <= dataSize) {
            std::cout << "입력제한 1023자를 넘어섰습니다. 다시 입력해주세요.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<int>::max(), '\n');
            continue;
        }

        std::cout << "보내기 완료.\n\n";
        std::cout << "----------------------------------------\n\n";

        // 끝나는지?
        if (0 == std::strcmp(sendData.c_str(), "quit")) {
            break;
        }

        // 문자열 보내기
        if (SOCKET_ERROR == ::send(clientSocket, sendData.c_str(), dataSize, 0)) {
            HandleSocketError();
            ::exit(EXIT_FAILURE);
        }

        // 보냈으면 받기
        // recv함수는 send함수와는 다르게 받은 데이터의 길이(수신한 바이트)를 반환함.
        // 0 -> 연결이 종료됨, SOCKET_ERROR -> 에러
        std::cout << "----------------------------------------\n\n";
        std::cout << "수신 대기...\n\n";
        int len = ::recv(clientSocket, recvData, RECV_SIZE, 0);
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

        // 다 받으면 다시 반복
    }
}

// IPv4를 사용하는 서버
void TCPServerIPv4()
{
    // socket 생성
    // AF_INET -> IPv4, SOCK_STREAM -> TCP 소켓, 프로토콜 0 -> 자동으로 결정
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    // sockaddr_in 구조체를 이용해 ip 주소, port 번호를 소켓에 바인딩
    sockaddr_in address;
    std::memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;                       // ip 프로토콜 (ip 버전)
    address.sin_port = ::htons(PORT_NUMBER);            // 포트 번호
    address.sin_addr.s_addr = ::htonl(INADDR_ANY);      // 서버로 들어오는 모든 ip를 받도록 설정

    // 위 코드에서 입력한 내용을 소켓에 바인딩
    if (SOCKET_ERROR == ::bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in))) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    // 소켓을 listen 상태로 변경 -> 수신 대기 상태로 변경
    // 소켓과 backlog를 받는다, 
    // backlog -> 해당 소켓에 PENDING, 해당 소켓에 연결되려고 기다리는 소켓의 최대 숫자라고 생각
    if (SOCKET_ERROR == listen(socket, SOMAXCONN)) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    sockaddr_in clientAddr;
    int clientAddrLen{ sizeof(sockaddr_in) };
    // 실제 연결 대기중인 소켓과 연결, 결과 값으로 소켓을 반환
    SOCKET clientSocket = ::accept(socket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
    if (INVALID_SOCKET == clientSocket) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    char ipv4Address[INET_ADDRSTRLEN]{ };
    ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipv4Address, INET_ADDRSTRLEN);
    std::cout << "[클라이언트 접속] IPv6: " << ipv4Address << ", PORT: " << ::ntohs(clientAddr.sin_port) << "\n";
    /* ------------------------ 여기까지 무사히 실행 되었다면 연결완료 ------------------------ */

    SendAndRecv(clientSocket);

    // 소켓 연결의 종료를 알리는 함수, 소켓 뒤의 파라미터는 어떤 작업(SEND?, RECV?, BOTH?)을 중단할 것인지 설정
    ::shutdown(clientSocket, SD_BOTH);

    // 생성된 소켓 닫기
    if (SOCKET_ERROR == ::closesocket(socket)) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }
}

// IPv6를 사용하는 서버
void TCPServerIPv6()
{
    // socket 생성
    // AF_INET -> IPv4, SOCK_STREAM -> TCP 소켓, 프로토콜 0 -> 자동으로 결정
    SOCKET socket = ::socket(AF_INET6, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    // sockaddr_in 구조체를 이용해 ip 주소, port 번호를 소켓에 바인딩
    sockaddr_in6 address;
    std::memset(&address, 0, sizeof(sockaddr_in6));
    address.sin6_family = AF_INET6;                       // ip 프로토콜 (ip 버전)
    address.sin6_port = ::htons(PORT_NUMBER);             // 포트 번호
    address.sin6_addr = in6addr_any;                      // 서버로 들어오는 모든 ip를 받도록 설정

    // 위 코드에서 입력한 내용을 소켓에 바인딩
    if (SOCKET_ERROR == ::bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in6))) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    // 소켓을 listen 상태로 변경 -> 수신 대기 상태로 변경
    // 소켓과 backlog를 받는다, 
    // backlog -> 해당 소켓에 PENDING, 해당 소켓에 연결되려고 기다리는 소켓의 최대 숫자라고 생각
    if (SOCKET_ERROR == listen(socket, SOMAXCONN)) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }

    sockaddr_in6 clientAddr;
    int clientAddrLen{ sizeof(sockaddr_in6) };
    // 실제 연결 대기중인 소켓과 연결, 결과 값으로 소켓을 반환
    SOCKET clientSocket = ::accept(socket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
    if (INVALID_SOCKET == clientSocket) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }
    
    char ipv6Address[INET6_ADDRSTRLEN]{ };
    ::inet_ntop(AF_INET6, &clientAddr.sin6_addr, ipv6Address, INET6_ADDRSTRLEN);
    std::cout << "[클라이언트 접속] IPv6: " << ipv6Address << ", PORT: " << ::ntohs(clientAddr.sin6_port) << "\n";
    /* ------------------------ 여기까지 무사히 실행 되었다면 연결완료 ------------------------ */

    SendAndRecv(clientSocket);

    // 소켓 연결의 종료를 알리는 함수, 소켓 뒤의 파라미터는 어떤 작업(SEND?, RECV?, BOTH?)을 중단할 것인지 설정
    ::shutdown(clientSocket, SD_BOTH);

    // 생성된 소켓 닫기
    if (SOCKET_ERROR == ::closesocket(socket)) {
        HandleSocketError();
        ::exit(EXIT_FAILURE);
    }
}

int main()
{
    WinSockStart();

    TCPServerIPv6();

    WinSockClean();
    // 시작 프로젝트를 여러개로 지정했을 때 바로 끝나지 않게 하기위해 작성
    char ch{ };
    std::cout << "끝내려면 엔터키를 눌러주세요.";
    ch = ::getchar();
}
