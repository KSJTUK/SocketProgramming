#pragma once

/* ----------------------------------------
*				TCPServerCore
* 
* �������� �� �⺻���� ����, ���ἳ��, �÷��̾� �߰�,
* ����� ������ Ŭ����
* 
* Ư���� ���� ���ٸ� ��ü�� �ϳ��� ����
*
  ---------------------------------------- */

#include "Client.h"

class TCPServerCore {
public:
	TCPServerCore();
	~TCPServerCore();

public:
	/** �ʱ�ȭ **/
	void Init();
	/** ������� �Ҹ��� **/
	void Join();

	/** Getter **/
	std::vector<Client>& GetClients();

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
			if (mClients[id].GetState() == CLIENT_STATE::EXITED) {
				continue;
			}
			mClients[id].Send(&packet);
		}
	}

private:
	/* ���ἳ���� �ʿ��� �Լ��� */
	void CreateCoreObjects();
	void StartAccept();

private:
	std::unique_ptr<class Listener> mListener;
	std::vector<std::thread> mClientServiceThreads;

	// Ŭ���̾�Ʈ�� ������ �����صдٸ� ���� map�� �� �ʿ䰡 �ִ°�?
	// - 24.09.23 std::vector�� ����
	/* ���� ���� Vector */
	std::vector<Client> mClients;
	// �ڷ� ������ ������ ��ȣ�ϱ� ���� mutex
};