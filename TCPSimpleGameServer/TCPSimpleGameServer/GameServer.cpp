#include "pch.h"
#include "GameServer.h"
#include "Listener.h"
#include "Timer.h"

/* ----------------------------------------
*
*				GameServer
*
  ---------------------------------------- */

GameServer::GameServer() = default;
GameServer::~GameServer() = default;

void GameServer::ProcessPacket(char* packet)
{
    // PacketBase ����ü���� 1����Ʈ�� size, 1����Ʈ�� type, 1����Ʈ�� �۽��� id�� ��������
    byte size = packet[0];
    byte type = packet[1];
    byte senderId = packet[2];

    switch (type) {
    case PACKET_PLAYER_CONNECT:
        Broadcast<PacketPlayerJoin>(PACKET_PLAYER_JOIN, senderId, packet);
        SendOtherClientsSession(senderId);
        break;

    case PACKET_PLAYER_INPUT:
        {
            PacketPlayerInput* input = reinterpret_cast<PacketPlayerInput*>(packet);
            mClients[senderId]->GetInputProcessor().KeyUpdate(input->keyInput, input->down);
        }
        break;

    case PACKET_PLAYER_JOIN:
        break;

    case PACKET_PLAYER_EXIT:
		Broadcast<PacketPlayerExit>(PACKET_PLAYER_EXIT, senderId, packet);
        ExitClient(senderId);
        break;

    case PACKET_PING:
        //Send<PacketPing>(PACKET_PING, senderId, packet);
        break;
    }
}

void GameServer::Update()
{
    static float updateDelay = 0.f;
    static const float delayed = 1.0f / 60.0f;
	mTimer->Update();
	mDeltaTime = mTimer->GetDeltaTime();

    updateDelay += mDeltaTime;
    if (updateDelay < delayed) {
        return;
    }
    mDeltaTime = updateDelay;

    PacketPosition2D packet{ sizeof(PacketPosition2D), PACKET_POSITION2D, };
    for (int clientId = 0; clientId < MAX_CLIENT; ++clientId) {
        mClients[clientId]->Update(mDeltaTime.load());

        std::lock_guard stateGuard{ mClients[clientId]->GetMutex() };
        if (mClients[clientId]->GetState() == CLIENT_STATE::JOINED) {
            std::unique_lock broadcastGuard{ mBroadcastLock };
			packet.pos = mClients[clientId]->GetPosition();
            Broadcast<PacketPosition2D>(PACKET_POSITION2D, clientId, reinterpret_cast<char*>(&packet));
            broadcastGuard.unlock();
        }
    }
    updateDelay = 0.0f;
}

void GameServer::CreateCoreObjects()
{
    mListener = std::make_unique<Listener>(SERVER_IP, SERVER_PORT);
    mTimer = std::make_unique<Timer>();
    for (int id = 0; id < MAX_CLIENT; ++id) {
        mClients.emplace_back(std::make_shared<Client>());
    }
}

void GameServer::Accept()
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
    mAcceptThread = std::thread{ [=]() { Accept(); } };
}

void GameServer::Join()
{
    for (std::thread& thread : mClientServiceThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    mAcceptThread.join();
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


        std::cout << "[Ŭ���̾�Ʈ ����" << " ID: "<< (int)id << "] IP: " << hostInfo.ip << " | PORT: " << hostInfo.port << "\n";
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
            packet.pos = mClients[id]->GetPosition();
            mClients[targetId]->GetTransceiver().Send(&packet);
        }
    }
}