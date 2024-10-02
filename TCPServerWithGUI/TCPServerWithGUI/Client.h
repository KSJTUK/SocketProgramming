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

enum class CLIENT_STATE : char {
	JOINED,
	EXITED,
};

/* ----------------------------------------
*
*				Client
*		
* 실제로 연결된 주체를 표현할 클래스
* Send, Recv등의 자겁을 여기서 할 예정
*
* 09-09 수정 -> id, Position 값을 ClientSession이 가지도록 수정
* Client클래스는 ClientSession 객체를 하나 가지도록 함
* 
* member:
* id
* session
* host info
* buffer
  ---------------------------------------- */

class Client {
	inline static constexpr int RECV_SIZE = 1024;

public:
	Client();
	~Client();

public:
	// Getter
	const Address::NetHostInfo& GetHostInfo() const { return mHostInfo; };
	const byte GetId() const;
	std::pair<float, float> GetPosition() const;
	CLIENT_STATE GetState() const { return mClientState; }

	std::mutex& GetMutex() { return mStateLock; }

	void Join(SOCKET clientSocket, byte id);
	void Exit();
	void CloseSocket();

	void Recv();

	// 위에서의 단점을 극복하기 위해 템플릿으로 설계
	template<typename PacketType, typename... Args> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(byte type, Args&&... args)
	{
		PacketType packet{ sizeof(PacketType), type, GetId(), (args)...};
		int result = ::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);

		if (result == SOCKET_ERROR) {
			gIOLock.lock();
			std::cout << "err" << (int)GetId() << std::endl;
			std::cout << GetErrorString() << std::endl;
			gIOLock.unlock();
		}
	}

	template <typename PacketType> requires std::is_base_of_v<PacketBase, PacketType>
	void Send(byte type, char* data)
	{
		PacketType packet;
		std::memcpy(&packet, data, sizeof(PacketType));
		packet.size = sizeof(PacketType);
		packet.type = type;
		packet.senderId = GetId();

		int result = ::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);

		if (result == SOCKET_ERROR) {
			gIOLock.lock();
			std::cout << "err" << (int)GetId() << std::endl;
			std::cout << GetErrorString() << std::endl;
			gIOLock.unlock();
		}
	}

	// 같은 패킷을 보내는 경우 계속해서 만들필요는 없음.
	void Send(PacketBase* packet)
	{
		int result = ::send(
			mSocket,
			reinterpret_cast<char*>(packet),
			packet->size,
			0
		);

		if (result == SOCKET_ERROR) {
			gIOLock.lock();
			std::cout << "err" << (int)GetId() << std::endl;
			std::cout << GetErrorString() << std::endl;
			gIOLock.unlock();
		}
	}

private:
	void ProcessPacket(char* packet);

private:
	SOCKET mSocket{ };
	std::unique_ptr<class ClientSession> mSession;
	Address::NetHostInfo mHostInfo{ };

	char mRecvBuffer[RECV_SIZE]{ };
	std::mutex mStateLock;

	CLIENT_STATE mClientState{ CLIENT_STATE::EXITED };

};
