#pragma once

/* ----------------------------------------
*				GameServer
*
* �������� �� �⺻���� ����, ���ἳ��, �÷��̾� �߰�,
* ����� ������ Ŭ����
*
* Ư���� ���� ���ٸ� ��ü�� �ϳ��� ����
*
  ---------------------------------------- */

#include "Client.h"

class Object;

class GameServer {
public:
	GameServer();
	~GameServer();

public:
	/** �ʱ�ȭ **/
	void Init();
	/** ������� �Ҹ��� **/
	void Join();

	/** Getter **/
	std::vector<std::shared_ptr<Client>>& GetClients();

	/** Ŭ���̾�Ʈ ���� ���� ó�� **/
	byte AddClient(SOCKET clientSocket);
	void ExitClient(byte id);

	/** ���� ���� �÷��̾�� ���� �÷��̾��� ������ ���� **/
	void SendOtherClientsSession(byte targetId);

	/* ��� Ŭ���̾�Ʈ���� ���� ��Ŷ�� ���� */
	template <typename PacketType> requires std::is_base_of_v<PacketBase, PacketType>
	void Broadcast(byte type, byte senderId, char* data)
	{
		PacketType packet;
		std::memcpy(&packet, data, sizeof(PacketType));
		packet.size = sizeof(PacketType);
		packet.type = type;
		packet.senderId = senderId;

		for (byte id = 0; id < MAX_CLIENT; ++id) {
			if (senderId == id) {
				mClients[id]->GetTransceiver().Send(&packet);
				continue;
			}

			std::lock_guard stateLock{ mClients[id]->GetMutex() };
			if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
				mClients[id]->GetTransceiver().Send(&packet);
			}
		}
	}

	void ProcessPacket(char* packet);

	void SendClientsInfo();
	void SendObjectsInfo();
	Object* CreateObject(OBJECT_TYPE objType, Vec2D pos, SizeF size, DWORD color = RGB(255, 255, 255));
	bool AllocObject(OBJECT_TYPE objType, Vec2D pos, SizeF size, DWORD color=RGB(255, 255, 255));

public:
	/* ���� ���� �۾� ���� �Լ��� */
	void Update();

private:
	/* ���ἳ���� �ʿ��� �Լ��� */
	void CreateCoreObjects();
	void Accept();

public:
	inline static std::atomic<float> mDeltaTime{ };

private:
	std::unique_ptr<class Listener> mListener;
	std::unique_ptr<class Timer> mTimer;

	std::mutex mBroadcastLock;

	std::thread mAcceptThread;
	std::vector<std::thread> mClientServiceThreads;

	std::vector<std::shared_ptr<Client>> mClients;
	std::array<bool, MAX_OBJECT> mObjectAlive{ };
	std::vector<std::unique_ptr<Object>> mObjects;
};