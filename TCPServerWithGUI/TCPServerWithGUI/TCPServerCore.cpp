#include "pch.h"
#include "TCPServerCore.h"
#include "Listener.h"

TCPServerCore::TCPServerCore() = default;
TCPServerCore::~TCPServerCore() = default;

void TCPServerCore::CreateCoreObjects()
{
	mListener = std::make_unique<Listener>(SERVER_IP, SERVER_PORT);
}

void TCPServerCore::StartAccept()
{
	while (true) {
		SOCKET clientSocket = mListener->Accept();
		if (INVALID_SOCKET == clientSocket) {
			continue;
		}

		mClientServiceThreads.push_back(std::thread{[=]()
			{
			    auto [ip, port] = Address::GetHostInfo(clientSocket);
	            std::cout << "[클라이언트 접속] IP: " << ip << " | PORT: " << port << "\n";

                RecvAndEcho(clientSocket);

                std::cout << "[클라이언트 연결 종료] IP: " << ip << " | PORT: " << port << "\n";
			}
		});
	}
}

void TCPServerCore::Init()
{
    WSAData wsaData;
    ASSERT_CRASH(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "WSAStartup Failure");

    CreateCoreObjects();
    StartAccept();
}

void TCPServerCore::Join()
{
    for (std::thread& thread : mClientServiceThreads) {
        thread.join();
    }

    ::WSACleanup();
}

void TCPServerCore::RecvAndEcho(SOCKET clientSocket)
{
    char buffer[1024]{ };

    while (true) {
        std::memset(buffer, 0, 1024);

        int len = ::recv(clientSocket, buffer, 1024, 0);
        if (len == 0) {
            ::shutdown(clientSocket, SD_BOTH);
            break;
        }
        else if (len < 0) {
            break;
        }

        std::cout << "[수신 " << len << "bytes]: " << buffer << "\n";

        if (SOCKET_ERROR == ::send(clientSocket, buffer, (int)(strlen(buffer)), 0)) {
            ::shutdown(clientSocket, SD_BOTH);
            break;
        }
    }
}
