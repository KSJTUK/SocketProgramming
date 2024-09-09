#pragma once

class ServerService {
	inline static constexpr int RECV_SIZE = 1024;

public:
	ServerService();
	~ServerService();

public:
	byte GetId() const { return mId; }

	bool ConnectToServer();
	void CreateRecvThread();

	void Join();

	// 위에서의 단점을 극복하기 위해 템플릿으로 설계
	template<typename PacketType, typename... Args> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(byte type, Args&&... args)
	{
		PacketType packet{ sizeof(PacketType), type, (args)... };
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

