#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <thread>
#include <iostream>
#include <mutex>

#include "Protocol.h"
#include "Macros.h"
#include "Random.h"

using namespace std::literals;

std::mutex gIOLock;

void Recv(SOCKET socket)
{
    char buffer[1024]{ };

    while (true) {
        ::memset(buffer, 0, 1024);

        int len = ::recv(socket, buffer, 1024, 0);
        if (0 == len) {
            std::cout << "연결종료\n";
            break;
        }
        else if (len < 0) {
            break;
        }

        byte size = buffer[0];
        byte type = buffer[1];
        switch (type) {
        case PACKET_POSITION2D:
        {
            PacketPosition2D* packet = reinterpret_cast<PacketPosition2D*>(buffer);
            {
                std::lock_guard ioLockGuard{ gIOLock };
                std::cout << "[수신] POSITION: (" << packet->x << ", " << packet->y << ")\n";
            }
        }
        break;

        default:
            break;
        };
    }
}

int main()
{
    WSAData wsaData;
    ASSERT_CRASH(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "");

    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_CRASH(INVALID_SOCKET != socket, "Socket is not Created");

    sockaddr_in address;
    std::memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = ::htons(SERVER_PORT);
    int ptonResult = ::inet_pton(AF_INET, SERVER_IP, &address.sin_addr.s_addr); // ip 주소
    if (0 == ptonResult) {
        ::exit(EXIT_FAILURE);
    }
    else if (-1 == ptonResult) {
        ::exit(EXIT_FAILURE);
    }

    if (SOCKET_ERROR == ::connect(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in))) {
        ::exit(EXIT_FAILURE);
    }

    std::thread recvThread{ Recv, socket };

    PacketPosition2D posPacket;
    // 데이터 송신 루프
    while (true) {
        posPacket.size = sizeof(PacketPosition2D);
        posPacket.type = PACKET_POSITION2D;
        posPacket.x = Random::GetUniformRandom(0, 1900);
        posPacket.y = Random::GetUniformRandom(0, 1000);

        // 보낼 문자열 입력 받기 및, 입력제한 조건 설정
        ASSERT_CRASH(SOCKET_ERROR != ::send(socket, reinterpret_cast<char*>(&posPacket), posPacket.size, 0), "");
        {
            std::lock_guard ioLockGuard{ gIOLock };
            std::cout << "[송신] POSITION: (" << posPacket.x << ", " << posPacket.y << ")\n";
        }

        std::this_thread::sleep_for(1000ms);
    }

    recvThread.join();

    ::shutdown(socket, SD_BOTH);
    ::closesocket(socket);

    ::WSACleanup();
}
