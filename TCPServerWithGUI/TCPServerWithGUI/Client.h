#pragma once

class Client {
	inline static constexpr int RECV_SIZE = 1024;

public:
	Client();
	explicit Client(unsigned short cid, SOCKET socket, const Address::NetHostInfo& hostInfo);
	~Client();

public:
	const Address::NetHostInfo& GetHostInfo() const { return mHostInfo; };

	void Send(PacketBase* packet);
	void Recv();

private:
	void BroadCasePacket(byte senderId, PacketBase* packet);
	void ProcessPacket(char* packet);

private:
	int mClientId{ };
	SOCKET mSocket{ };
	Address::NetHostInfo mHostInfo{ };
	char mRecvBuffer[RECV_SIZE]{ };
};
