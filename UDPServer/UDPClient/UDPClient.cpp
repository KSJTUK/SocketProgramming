#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

constexpr unsigned short PORT = 7777;
constexpr int SEND_SIZE = 1024;
constexpr int BUFFER_SIZE = 1024;

// 에러 핸들링
void HandleErrorDisplay()
{

}

void HandleErrorQuit()
{
    HandleErrorDisplay();
    ::exit(EXIT_FAILURE);
}

// 소켓 dll 초기화, 종료
void WinSockStart()
{
    WSAData wsaData;
    if (0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        HandleErrorQuit();
    }
}

void WinSockClean()
{
    ::WSACleanup();
}

// IPv4를 사용하는 UDP 서버
void UDPClientIPv4()
{
    SOCKET udpSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == udpSocket) {
        HandleErrorQuit();
    }

    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = ::htons(PORT);
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    sockaddr_in peerAddress;
    int peerAddrLength{ sizeof(peerAddress) };

    int recvResult;
    char dataBuffer[BUFFER_SIZE + 1]{ };
    char addressBuffer[INET_ADDRSTRLEN];
    while (true) {
        std::memset(&peerAddress, 0, sizeof(peerAddress));

        std::cin >> dataBuffer;

        // udp 함수인 sendto는 연결설정을 하지 않기 때문에 함수 자체에 주소를 지정한다.
        if (SOCKET_ERROR == ::sendto(udpSocket, dataBuffer, (int)strlen(dataBuffer), 0, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress))) {
            HandleErrorDisplay();
            break;
        }

        // recvfrom에서 보낸 이의 정보를 알 수 있고 sendto에 지정한 주소와 비교하여 같은 주소인지 알 수 있음.
        recvResult = ::recvfrom(udpSocket, dataBuffer, BUFFER_SIZE, 0, reinterpret_cast<sockaddr*>(&peerAddress), &peerAddrLength);

        if (SOCKET_ERROR == recvResult) {
            HandleErrorDisplay();
            break;
        }

        // 송신자 IP수소체크 -> 다르면? 데이터를 송신한 주소와 수신받은 데이터의 주소가 서로 상이
        if (std::memcmp(&peerAddress, &serverAddress, sizeof(sockaddr_in))) {
            std::cout << "[오류] 서로 다른 주소 (잘못된 데이터 수신)" << std::endl;
            break;
        }

        dataBuffer[recvResult] = '\0';
        std::cout << "[수신 " << recvResult << "bytes]: " << dataBuffer << "\n";
    }

    ::closesocket(udpSocket);
}

int main()
{
    WinSockStart();

    UDPClientIPv4();

    WinSockClean();
}