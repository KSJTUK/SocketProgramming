#pragma once

namespace Address {
	struct NetHostInfo {
		char ip[INET_ADDRSTRLEN];
		unsigned short port;
	};

	inline NetHostInfo GetHostInfo(SOCKET socket)
	{
		NetHostInfo hostInfo{ };

		sockaddr_in address;
		int addressLength{ sizeof(sockaddr_in) };
		::getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressLength);

		::inet_ntop(AF_INET, &address.sin_addr, hostInfo.ip, INET_ADDRSTRLEN);
		hostInfo.port = ::ntohs(address.sin_port);

		return hostInfo;
	}
}

class Transceiver {
	inline static constexpr int RECV_SIZE = 1024;

public:
	Transceiver();
	~Transceiver();

public:
	// Getter
	const Address::NetHostInfo& GetHostInfo() const { return mHostInfo; };

public:
	void Init(SOCKET socket);
	void CloseSocket();

	void Recv();

	// �������� ������ �غ��ϱ� ���� ���ø����� ����
	template<typename PacketType, typename... Args> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(byte type, byte id, Args&&... args)
	{
		PacketType packet{ sizeof(PacketType), type, id, (args)... };
		int result = ::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);
	}

	template <typename PacketType> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(byte type, byte id, char* data)
	{
		PacketType packet;
		std::memcpy(&packet, data, sizeof(PacketType));
		packet.size = sizeof(PacketType);
		packet.type = type;
		packet.senderId = id;

		int result = ::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);
	}

	// ���� ��Ŷ�� ������ ��� ����ؼ� �����ʿ�� ����.
	void Send(PacketBase* packet)
	{
		int result = ::send(
			mSocket,
			reinterpret_cast<char*>(packet),
			packet->size,
			0
		);
	}

private:
	void ProcessPacket(char* packet);

private:
	SOCKET mSocket{ };
	Address::NetHostInfo mHostInfo{ };
	char mRecvBuffer[RECV_SIZE]{ };
};

