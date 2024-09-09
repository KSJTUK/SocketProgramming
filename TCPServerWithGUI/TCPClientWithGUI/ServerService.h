#pragma once

/* ----------------------------------------
*				ServerService
* 
* 기본적인 서버로 송신, 서버로부터 수신, 연결설정
* , 패킷처리 등을 담당하는 클래스
  ---------------------------------------- */


class ServerService {
	inline static constexpr int RECV_SIZE = 1024;

public:
	ServerService();
	~ServerService();

public:
	byte GetId() const { return mId; }

	// 서버와 연결설정, 수신 쓰레드 생성하는 함수
	bool ConnectToServer();
	void CreateRecvThread();

	// Thread Join
	void Join();

	// 기존 Send함수는 패킷을 생성 후 Send함수를 작성하는 단점을 해결하기 위해 템플릿으로 설계
	// Send함수내에서 packet 생성
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

