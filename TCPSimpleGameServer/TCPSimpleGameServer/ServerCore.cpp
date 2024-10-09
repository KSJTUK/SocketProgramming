#include "pch.h"
#include "ServerCore.h"
#include "Listener.h"

/* ----------------------------------------
*
*				ServerCore
*
  ---------------------------------------- */

ServerCore::ServerCore() = default;
ServerCore::~ServerCore() = default;

void ServerCore::CreateCoreObjects()
{
    mListener = std::make_unique<Listener>(SERVER_IP, SERVER_PORT);
    for (int id = 0; id < MAX_CLIENT; ++id) {
        mClients.emplace_back(std::make_shared<Client>());
    }
}

void ServerCore::StartAccept()
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
                mClients[id]->Send<PacketPlayerConnect>(PACKET_PLAYER_CONNECT);

                mClients[id]->Recv();
            }
            });
    }
}

void ServerCore::Init()
{
    WSAData wsaData;
    ASSERT(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "WSAStartup Failure");

    CreateCoreObjects();
    StartAccept();
}

void ServerCore::Join()
{
    for (std::thread& thread : mClientServiceThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    ::WSACleanup();
}

std::vector<std::shared_ptr<Client>>& ServerCore::GetClients()
{
    return mClients;
}

byte ServerCore::AddClient(SOCKET clientSocket)
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

void ServerCore::ExitClient(byte id)
{
    std::lock_guard stateGuard{ mClients[id]->GetMutex() };
    if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
        auto& [ip, port] = mClients[id]->GetHostInfo();
        std::cout << "[Ŭ���̾�Ʈ ���� ����] IP: " << ip << " | PORT: " << port << "\n";
        mClients[id]->CloseSocket();
        mClients[id]->Exit();
    }
}

void ServerCore::SendOtherClientsSession(byte targetId)
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
