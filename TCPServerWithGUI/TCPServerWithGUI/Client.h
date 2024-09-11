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
