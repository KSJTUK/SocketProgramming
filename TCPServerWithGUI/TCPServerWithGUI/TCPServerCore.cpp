#include "pch.h"
#include "TCPServerCore.h"
#include "Listener.h"
#include "Client.h"

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

        short addedId = AddClient(clientSocket);

        mClientServiceThreads.push_back(std::thread{ [=]()
            {
                short id = addedId;
                Client& client = mClients[id];

                client.Recv();
            
                ExitClient(id);
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

std::unordered_map<unsigned short, Client>& TCPServerCore::GetClients()
{
    return mClients;
}

short TCPServerCore::AddClient(SOCKET clientSocket)
{
    for (unsigned short id = 0; id < 64; ++id) {
        if (not mClients.contains(id)) {
            Address::NetHostInfo hostInfo = Address::GetHostInfo(clientSocket);

            mClients.emplace(id, Client{ id, clientSocket, hostInfo });
            std::cout << "[클라이언트 접속] IP: " << hostInfo.ip << " | PORT: " << hostInfo.port << "\n";
            return id;
        }
    }

    return -1;
}

void TCPServerCore::ExitClient(unsigned short id)
{
    if (mClients.contains(id)) {
        auto& [ip, port] = mClients[id].GetHostInfo();
        std::cout << "[클라이언트 연결 종료] IP: " << ip << " | PORT: " << port << "\n";
        mClients.erase(id);
    }
}