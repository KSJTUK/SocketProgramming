#include "pch.h"
#include "TCPServerCore.h"
#include "Listener.h"
#include "Client.h"

/* ----------------------------------------
*
*				TCPServerCore
*
  ---------------------------------------- */

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

        if (mClients.size() >= 64) {
            continue;
        }

        byte addedId = AddClient(clientSocket);

        // 람다를 이용해 생성
        // 처음 연결시 클라이언트에게 ID를 부여하는 패킷 전송
        mClientServiceThreads.push_back(std::thread{ [=]()
            {
                byte id = addedId;
                Client& client = mClients[id];

                client.Send<PacketPlayerConnect>(PACKET_PLAYER_CONNECT);

                client.Recv();
            
                ExitClient(id);
            }
		});
	}
}

void TCPServerCore::Init()
{
    WSAData wsaData;
    ASSERT(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "WSAStartup Failure");

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

std::unordered_map<byte, Client>& TCPServerCore::GetClients()
{
    return mClients;
}

byte TCPServerCore::AddClient(SOCKET clientSocket)
{
    for (byte id = 0; id < 64; ++id) {
        if (not mClients.contains(id)) {
            Address::NetHostInfo hostInfo = Address::GetHostInfo(clientSocket);

            mClients.emplace(id, std::move(Client{ id, clientSocket, hostInfo }));
            std::cout << "[클라이언트 접속] IP: " << hostInfo.ip << " | PORT: " << hostInfo.port << "\n";
            return id;
        }
    }

    return 0xFF;
}

void TCPServerCore::ExitClient(byte id)
{
    if (mClients.contains(id)) {
        auto& [ip, port] = mClients[id].GetHostInfo();
        std::cout << "[클라이언트 연결 종료] IP: " << ip << " | PORT: " << port << "\n";
        mClients.erase(id);
    }
}