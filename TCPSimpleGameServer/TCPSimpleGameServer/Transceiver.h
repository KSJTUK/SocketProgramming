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

/* ----------------------------------------
*
*				Transceiver
* Client 의 송수신 기능 분리
*
  ---------------------------------------- */

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

	// 위에서의 단점을 극복하기 위해 템플릿으로 설계
	template<typename PacketType, typename... Args> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(const byte type, const byte id, Args&&... args)
	{
		PacketType packet{ sizeof(PacketType), type, id, (args)... };
		int result = ::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);

		if (SOCKET_ERROR == result) {
			//std::cout << "[Client " << id << "] Occured Socket Error: Send Error" << std::endl;
		}
	}

	template <typename PacketType> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(const byte type, const byte id, char* data)
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

		if (SOCKET_ERROR == result) {
			//std::cout << "[Client " << id << "] Occured Socket Error: Send Error" << std::endl;
		}
	}

	// 같은 패킷을 보내는 경우 계속해서 만들필요는 없음.
	void Send(PacketBase* const packet)
	{
		int result = ::send(
			mSocket,
			reinterpret_cast<char*>(packet),
			packet->size,
			0
		);

		if (SOCKET_ERROR == result) {
			//std::cout << "[Client " << packet->senderId << "] Occured Socket Error: Send Error" << std::endl;
		}
	}

private:
	SOCKET mSocket{ };
	Address::NetHostInfo mHostInfo{ };
	char mRecvBuffer[RECV_SIZE]{ };
};

