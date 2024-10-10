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

        // ���ٸ� �̿��� ����
        // ó�� ����� Ŭ���̾�Ʈ���� ID�� �ο��ϴ� ��Ŷ ����
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
            // Ŭ���̾�Ʈ�� ���¸� �����ϴ� �κ��� locking
            std::lock_guard stateGuard{ mClients[id]->GetMutex() };
            if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
                continue;
            }
            mClients[id]->Join(clientSocket, id);
        }

        Address::NetHostInfo hostInfo = Address::GetHostInfo(clientSocket);


        std::cout << "[Ŭ���̾�Ʈ ����] IP: " << hostInfo.ip << " | PORT: " << hostInfo.port << "\n";
        return id;
    }

    return NULL_CLIENT_ID;
}

void GameServer::ExitClient(byte id)
{
    std::lock_guard stateGuard{ mClients[id]->GetMutex() };
    if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
		auto& [ip, port] = mClients[id]->GetTransceiver().GetHostInfo();
        std::cout << "[Ŭ���̾�Ʈ ���� ����] IP: " << ip << " | PORT: " << port << "\n";
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
