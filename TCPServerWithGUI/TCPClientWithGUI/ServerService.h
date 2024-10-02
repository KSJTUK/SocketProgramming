#pragma once

/* ----------------------------------------
*				ServerService
* 
* 기본적인 서버로 송신, 서버로부터 수신, 연결설정,
* 패킷처리 등을 담당하는 클래스
* 
  ---------------------------------------- */

class ServerService {
	// 데이터 수신 버퍼의 크기
	inline static constexpr int RECV_SIZE = 1024;
	// 연결실패시 연결을 다시 시도해볼 횟수
	inline static constexpr int MAX_TRY_CONNECT = 2;

public:
	ServerService();
	~ServerService();

public:
	byte GetId() const volatile { return mId; }

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

		int sendResult = ::send(
			mSocket,
			reinterpret_cast<char*>(&packet),
			packet.size,
			0
		);
		
		if (SOCKET_ERROR == sendResult) {
			::MessageBoxA(nullptr, "Send Error", GetErrorString().c_str(), MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
		}
	}

private:
	void ProcessPacket(char* packet);
	void Recv();

private:
	SOCKET mSocket{ INVALID_SOCKET };
	std::thread mRecvThread{ };

	char mRecvBuffer[RECV_SIZE]{ };

	byte mId{ 0xFF };
	volatile bool mRecvThreadRunning{ true };
};

