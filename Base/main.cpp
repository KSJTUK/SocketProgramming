#include <iostream>

// 소켓 프로그래밍을 위한 헤더
#include <WinSock2.h>
#include <WS2tcpip.h>

// ws2_32 라이브러리, ws2_32.dll과 링크
#pragma comment(lib, "ws2_32.lib")

// 사용할 포트 번호 정의, 포트 번호는 16비트 정수를 이용 (unsigned short)
constexpr unsigned short PORT_NUMBER = 7777;
// 테스트용으로 보낼 문자열의 최대 길이 정의
constexpr unsigned short SEND_SIZE = 1024;

int main()
{
    // WSAStartup 윈도우 소켓 초기화를 위한 변수, 사용되는 일은 적음
    WSAData wsaData;

    if (0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        // 에러 핸들링 추가
        return EXIT_FAILURE;
    }

    // socket 생성
    // AF_INET -> IPv4, SOCK_STREAM -> TCP 소켓, 프로토콜 0 -> 자동으로 결정
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket) {
        // 에러 핸들링 추가
        return EXIT_FAILURE;
    }

    // sockaddr_in 구조체를 이용해 ip 주소, port 번호를 소켓에 바인딩
    sockaddr_in address;
    std::memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;                       // ip 프로토콜 (ip 버전)
    address.sin_port = ::htons(PORT_NUMBER);            // 포트 번호
    address.sin_addr.s_addr = ::htonl(INADDR_ANY);      // 서버로 들어오는 모든 ip를 받도록 설정
    
    // 위 코드에서 입력한 내용을 소켓에 바인딩
    if (SOCKET_ERROR == ::bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in))) {
        // 에러 핸들링 추가
        return EXIT_FAILURE;
    }

    // 소켓을 listen 상태로 변경 -> 수신 대기 상태로 변경
    // 소켓과 backlog를 받는다, 
    // backlog -> 해당 소켓에 PENDING, 해당 소켓에 연결되려고 기다리는 소켓의 최대 숫자라고 생각
    if (SOCKET_ERROR == listen(socket, 10)) {
        // 에러 핸들링 추가
        return EXIT_FAILURE;
    }

    sockaddr_in clientAddr;
    int clientAddrLen{ sizeof(sockaddr_in) };
    // 실제 연결 대기중인 소켓과 연결, 결과 값으로 소켓을 반환
    SOCKET clientSocket = ::accept(socket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
    if (INVALID_SOCKET == clientSocket) {
        // 에러 핸들링 추가
        return EXIT_FAILURE;
    }
    /* ------------------------ 여기까지 무사히 실행 되었다면 연결완료 ------------------------ */

    // 테스트용 send 잘 가는지는 클라이언트 부분을 만들어서 확인!
    // 주의! 처음에 만든 소켓은 서버에서 클라이언트와 연결을 위한 소켓임, accept를 위해 생성한 소켓을 이용
    char sendData[SEND_SIZE]{ "Test Send Data To Client" };
    if (SOCKET_ERROR == ::send(clientSocket, sendData, SEND_SIZE, 0)) {
        // 에러 핸들링 추가
        return EXIT_FAILURE;
    }

    // 생성된 소켓 
    if (SOCKET_ERROR == ::closesocket(socket)) {
        // 에러 핸들링
        return EXIT_FAILURE;
    }

    // ws2_32.dll의 사용을 종료하는 함수
    // MSDN에서는 WSAStartup과 쌍을 이루어 호출하는 것을 권장하고 있음.
    // 자세한 설명은 아래 링크 참고
    // https://learn.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsacleanup
    if (0 != ::WSACleanup()) {
        // 에러 핸들링
        return EXIT_FAILURE;
    }

    // 시작 프로젝트를 여러개로 지정했을 때 바로 끝나지 않게 하기위해 작성
    char ch{ };
    std::cout << "끝내려면 엔터키를 눌러주세요.";
    ch = ::getchar();
}
