#pragma once

/* ----------------------------------------
*
*				Client
*		
* ������ ����� ��ü�� ǥ���� Ŭ����
* Send, Recv���� �ڰ��� ���⼭ �� ����
*
* 09-09 ���� -> id, Position ���� ClientSession�� �������� ����
* ClientŬ������ ClientSession ��ü�� �ϳ� �������� ��
* 
* member:
* id
* session
* host info
* buffer
  ---------------------------------------- */

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

enum class CLIENT_STATE : char {
	JOINED,
	EXITED,
};

class Client {
	inline static constexpr int RECV_SIZE = 1024;

public:
	Client();
	Client(Client&& other) noexcept;
	~Client();

public:
	// Getter
	const Address::NetHostInfo& GetHostInfo() const { return mHostInfo; };
	const byte GetId() const;
	std::pair<float, float> GetPosition() const;
	CLIENT_STATE GetState() const { return mClientState; }

	void Join(SOCKET clientSocket, byte id);
	void Exit();

	void Recv();

	// �������� ������ �غ��ϱ� ���� ���ø����� ����
	template<typename PacketType, typename... Args> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(byte type, Args&&... args)
	{
		PacketType packet{ sizeof(PacketType), type, GetId(), (args)...};
		::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);
	}

	template <typename PacketType> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(byte type, char* data)
	{
		PacketType packet;
		std::memcpy(&packet, data, sizeof(PacketType));
		packet.size = sizeof(PacketType);
		packet.type = type;

		::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);
	}

	// ���� ��Ŷ�� ������ ��� ����ؼ� �����ʿ�� ����.
	void Send(PacketBase* packet)
	{
		::send(
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
	std::unique_ptr<class ClientSession> mSession;
	Address::NetHostInfo mHostInfo{ };
	char mRecvBuffer[RECV_SIZE]{ };
	byte mRemainByte{ };
	CLIENT_STATE mClientState{ CLIENT_STATE::EXITED };

#if NETWORK_DEBUG
	std::mutex mIOLock{ };
#endif
};
