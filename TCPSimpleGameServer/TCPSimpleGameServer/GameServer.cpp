#include "pch.h"
#include "GameServer.h"
#include "Listener.h"

/* ----------------------------------------
*
*				GameServer
*
  ---------------------------------------- */

GameServer::GameServer() = default;
GameServer::~GameServer() = default;

void GameServer::CreateCoreObjects()
{
    mListener = std::make_unique<Listener>(SERVER_IP, SERVER_PORT);
    for (int id = 0; id < MAX_CLIENT; ++id) {
        mClients.emplace_back(std::make_shared<Client>());
    }
}

void GameServer::StartAccept()
{
    while (true) {
        SOCKET clientSocket = mListener->Accept();
        if (INVALID_SOCKET == clientSocket) {
            continue;
        }

        byte addedId = AddClient(clientSocket);
        if (NULL_CLIENT_ID == addedId) {
            continue;
        }

        // 람다를 이용해 생성
        // 처음 연결시 클라이언트에게 ID를 부여하는 패킷 전송
        mClientServiceThreads.push_back(std::thread{ [=]()
            {
                byte id = addedId;
                mClients[id]->GetTransceiver().Send<PacketPlayerConnect>(PACKET_PLAYER_CONNECT, id);

                mClients[id]->GetTransceiver().Recv();
            }
        });
    }
}

void GameServer::Init()
{
    WSAData wsaData;
    ASSERT(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "WSAStartup Failure");

    CreateCoreObjects();
    StartAccept();
}

void GameServer::Join()
{
    for (std::thread& thread : mClientServiceThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    ::WSACleanup();
}

std::vector<std::shared_ptr<Client>>& GameServer::GetClients()
{
    return mClients;
}

byte GameServer::AddClient(SOCKET clientSocket)
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

void GameServer::ExitClient(byte id)
{
    std::lock_guard stateGuard{ mClients[id]->GetMutex() };
    if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
		auto& [ip, port] = mClients[id]->GetTransceiver().GetHostInfo();
        std::cout << "[클라이언트 연결 종료] IP: " << ip << " | PORT: " << port << "\n";
        mClients[id]->GetTransceiver().CloseSocket();
        mClients[id]->Exit();
    }
}

void GameServer::SendOtherClientsSession(byte targetId)
{
    PacketPlayerJoin packet{ sizeof(PacketPlayerJoin), PACKET_PLAYER_JOIN };
    for (int id = 0; id < MAX_CLIENT; ++id) {
        std::lock_guard stateGuard{ mClients[id]->GetMutex() };
        if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
            packet.senderId = id;
            auto [x, y] = mClients[id]->GetPosition();
            mClients[targetId]->GetTransceiver().Send(&packet);
        }
    }
}

void GameServer::UpdatePlayer(byte id, const Position pos)
{
    mClients[id]->SetPosition(pos);
}
