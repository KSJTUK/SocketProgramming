#pragma once

/* ----------------------------------------
*				ServerService
* 
* �⺻���� ������ �۽�, �����κ��� ����, ���ἳ��
* , ��Ŷó�� ���� ����ϴ� Ŭ����
  ---------------------------------------- */


class ServerService {
	inline static constexpr int RECV_SIZE = 1024;

public:
	ServerService();
	~ServerService();

public:
	byte GetId() const { return mId; }

	// ������ ���ἳ��, ���� ������ �����ϴ� �Լ�
	bool ConnectToServer();
	void CreateRecvThread();

	// Thread Join
	void Join();

	// ���� Send�Լ��� ��Ŷ�� ���� �� Send�Լ��� �ۼ��ϴ� ������ �ذ��ϱ� ���� ���ø����� ����
	// Send�Լ������� packet ����
	template<typename PacketType, typename... Args> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(byte type, Args&&... args)
	{
		PacketType packet{ sizeof(PacketType), type, mId, (args)... };
		::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);
	}

private:
	void ProcessPacket(char* packet);
	void Recv();

private:
	SOCKET mSocket{ INVALID_SOCKET };
	std::thread mRecvThread{ };

	byte mId{ 0xFF };
	volatile bool mRecvThreadRunning{ true };

	char mRecvBuffer[RECV_SIZE]{ };
};

