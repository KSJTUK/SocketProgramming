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
    mClients.resize(MAX_CLIENT);
}

void TCPServerCore::StartAccept()
{
	while (true) {
		SOCKET clientSocket = mListener->Accept();
		if (INVALID_SOCKET == clientSocket) {
			continue;
		}

        byte addedId = AddClient(clientSocket);

        // ���ٸ� �̿��� ����
        // ó�� ����� Ŭ���̾�Ʈ���� ID�� �ο��ϴ� ��Ŷ ����
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

std::vector<Client>& TCPServerCore::GetClients()
{
    return mClients;
}

byte TCPServerCore::AddClient(SOCKET clientSocket)
{
    for (byte id = 0; id < MAX_CLIENT; ++id) {
        if (mClients[id].GetState() == CLIENT_STATE::EXITED) {
            Address::NetHostInfo hostInfo = Address::GetHostInfo(clientSocket);

            {
                std::lock_guard clientGuard{ mClientsLock };
                mClients[id].Join(clientSocket, id);
            }
            
            std::cout << "[Ŭ���̾�Ʈ ����] IP: " << hostInfo.ip << " | PORT: " << hostInfo.port << "\n";
            return id;
        }
    }

    return NULL_CLIENT_ID;
}

void TCPServerCore::ExitClient(byte id)
{
    if (mClients[id].GetState() == CLIENT_STATE::JOINED) {
        auto& [ip, port] = mClients[id].GetHostInfo();
        std::cout << "[Ŭ���̾�Ʈ ���� ����] IP: " << ip << " | PORT: " << port << "\n";
        
        {
            std::lock_guard clientGuard{ mClientsLock };
            mClients[id].Exit();
        }
    }
}

void TCPServerCore::SendOtherClientsSession(byte targetId)
{
    static std::vector<PacketPlayerJoin> packets(MAX_CLIENT);
    for (int id = 0; id < MAX_CLIENT; ++id) {
        if (mClients[id].GetState() == CLIENT_STATE::EXITED) {
            continue;
        }
        auto [x, y] = mClients[id].GetPosition();

        packets[id].type = PACKET_PLAYER_JOIN;
        packets[id].size = sizeof(PacketPlayerJoin);
        packets[id].senderId = id;
        packets[id].x = x;
        packets[id].y = y;
    }

    std::lock_guard clientGuard{ mClientsLock };
    // �ϴ� ������ Ŭ���̾�Ʈ���� id�� ���� JOIN ��Ŷ�� ��������
    // ��Ŷ�� �̸� ����� ���� �����⸸ ���� -> lock�� �� ������ ��ɾ �ּ�ȭ
    for (int id = 0; id < MAX_CLIENT; ++id) {
        if (mClients[id].GetState() == CLIENT_STATE::EXITED) {
            continue;
        }

        mClients[targetId].Send(&packets[id]);
    }
}
