#pragma once

class ServerService {
	inline static constexpr int RECV_SIZE = 1024;

public:
	ServerService();
	~ServerService();

public:
	bool ConnectToServer();

	void Send(PacketBase* packet);
	void CreateRecvThread();

private:
	void ProcessPacket(char* packet);
	void Recv();

private:
	SOCKET mSocket{ INVALID_SOCKET };
	std::thread mRecvThread{ };
	char mRecvBuffer[RECV_SIZE]{ };
};

