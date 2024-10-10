#include "pch.h"
#include "TCPServerCore.h"
#include "Listener.h"

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
    for (int id = 0; id < MAX_CLIENT; ++id) {
        mClients.emplace_back(std::make_shared<Client>());
    }
}

void TCPServerCore::StartAccept()
{
	while (true) {
		SOCKET clientSocket = mListener->Accept();
		if (INVALID_SOCKET == clientSocket) {
			continue;
		}

        byte addedId = AddClient(clientSocket);
        if (NULL_CLIENT_ID == addedId) {
            gIOLock.lock();
            std::cout << "NULL CLIENT ID" << std::endl;
            gIOLock.unlock();
            continue;
        }

        // 람다를 이용해 생성
        // 처음 연결시 클라이언트에게 ID를 부여하는 패킷 전송
        mClientServiceThreads.push_back(std::thread{ [=]()
            {
                byte id = addedId;
                mClients[id]->Send<PacketPlayerConnect>(PACKET_PLAYER_CONNECT);

                mClients[id]->Recv();
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
        if (thread.joinable()) {
            thread.join();
        }
    }

    ::WSACleanup();
}

std::vector<std::shared_ptr<Client>>& TCPServerCore::GetClients()
{
    return mClients;
}

byte TCPServerCore::AddClient(SOCKET clientSocket)
{
	for (byte id = 0; id < MAX_CLIENT; ++id) {
        {
            // 클라이언트의 상태를 변경하는 부분은 locking
			std::lock_guard stateGuard{ mClients[id]->GetMutex() };
            if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
                continue;
            }
            mClients[id]->Join(clientSocket, id);
        }

		Address::NetHostInfo hostInfo = Address::GetHostInfo(clientSocket);

		std::cout << "[클라이언트 접속] IP: " << hostInfo.ip << " | PORT: " << hostInfo.port << "\n";
		return id;
	}

    return NULL_CLIENT_ID;
}

void TCPServerCore::ExitClient(byte id)
{
	std::lock_guard stateGuard{ mClients[id]->GetMutex() };
    if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
        auto& [ip, port] = mClients[id]->GetHostInfo();
        std::cout << "[클라이언트 연결 종료] IP: " << ip << " | PORT: " << port << "\n";
        mClients[id]->CloseSocket();
        mClients[id]->Exit();
    }
}

void TCPServerCore::SendOtherClientsSession(byte targetId)
{
    PacketPlayerJoin packet{ sizeof(PacketPlayerJoin), PACKET_PLAYER_JOIN };
    for (int id = 0; id < MAX_CLIENT; ++id) {
        std::lock_guard stateGuard{ mClients[id]->GetMutex() };
		if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
			packet.senderId = id;
			auto [x, y] = mClients[id]->GetPosition();
			mClients[targetId]->Send(&packet);
        }
    }
}
