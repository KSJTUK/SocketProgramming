#pragma once


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
	explicit Client(byte cid, SOCKET socket, const Address::NetHostInfo& hostInfo);
	Client(Client&& other) noexcept;
	~Client();

public:
	// Getter
	const Address::NetHostInfo& GetHostInfo() const { return mHostInfo; };
	const byte GetId() const;
	std::pair<float, float> GetPosition() const;

	void Recv();

	// 위에서의 단점을 극복하기 위해 템플릿으로 설계
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

	// 같은 패킷을 보내는 경우 계속해서 만들필요는 없음.
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
	void SendOtherClientsSession(byte targetId);

	template <typename PacketType> requires std::is_base_of_v<PacketBase, PacketType>
	void BroadCasePacket(byte type, byte senderId, char* data)
	{
		PacketType packet;
		std::memcpy(&packet, data, sizeof(PacketType));
		packet.size = sizeof(PacketType);
		packet.type = type;
		packet.senderId = senderId;

		for (auto& [id, client] : gServerCore.GetClients()) {
			if (id == senderId) {
				continue;
			}

			client.Send(&packet);
		}
	}

private:
	SOCKET mSocket{ };
	std::unique_ptr<class ClientSession> mSession{ };
	Address::NetHostInfo mHostInfo{ };
	char mRecvBuffer[RECV_SIZE]{ };
};
