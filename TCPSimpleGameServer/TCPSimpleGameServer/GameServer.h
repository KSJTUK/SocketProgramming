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

	// �ӽ� �ڵ�, �и��� ����� �� �۵��ϴ��� Ȯ�ο�
	void UpdatePlayer(byte id, const Position pos);

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
			std::lock_guard stateLock{ mClients[id]->GetMutex() };
			if (mClients[id]->GetState() == CLIENT_STATE::EXITED) {
				continue;
			}

			mClients[id]->GetTransceiver().Send(&packet);
		}
	}

private:
	/* ���ἳ���� �ʿ��� �Լ��� */
	void CreateCoreObjects();
	void StartAccept();

private:
	std::unique_ptr<class Listener> mListener;
	std::vector<std::thread> mClientServiceThreads;

	std::vector<std::shared_ptr<Client>> mClients;
};