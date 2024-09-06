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
void UDPServerIPv4()
{
    SOCKET udpSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == udpSocket) {
        HandleErrorQuit();
    }

    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = ::htons(PORT);
    serverAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);

    if (SOCKET_ERROR == ::bind(udpSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress))) {
        HandleErrorQuit();
    }

    sockaddr_in clientAddress;
    int clientAddrLength{ sizeof(clientAddress) };

    int recvResult;
    char dataBuffer[BUFFER_SIZE + 1];
    char addressBuffer[INET_ADDRSTRLEN];
    while (true) {
		std::memset(&clientAddress, 0, sizeof(clientAddress));
        recvResult = ::recvfrom(udpSocket, dataBuffer, BUFFER_SIZE, 0, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddrLength);

        if (SOCKET_ERROR == recvResult) {
            HandleErrorDisplay();
            break;
        }

        dataBuffer[recvResult] = '\0';
        std::cout << "[클라이언트 IP: " << ::inet_ntop(AF_INET, &clientAddress.sin_addr, addressBuffer, INET_ADDRSTRLEN) <<
            " | PORT: " << ::ntohs(clientAddress.sin_port) << "]: " << dataBuffer << "\n";

        if (SOCKET_ERROR == ::sendto(udpSocket, dataBuffer, recvResult, 0, reinterpret_cast<sockaddr*>(&clientAddress), sizeof(clientAddress))) {
            HandleErrorDisplay();
            break;
        }
    }

    ::closesocket(udpSocket);
}

int main()
{
    WinSockStart();

    UDPServerIPv4();

    WinSockClean();
}