#include "pch.h"
#include "ServerService.h"
#include "GameFramework.h"
#include "Shape.h"
#include "Player.h"

/* ----------------------------------------
*
*				ServerService
*
  ---------------------------------------- */

ServerService::ServerService()
{
	WSAData wsaData;
	ASSERT(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "WSAStartup Failure");

	mSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	ASSERT(INVALID_SOCKET != mSocket, "Socket is not Created");
}

ServerService::~ServerService() = default;

// 서버와 연결설정
bool ServerService::ConnectToServer()
{
	// 서버 IP, PORT번호 설정
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(sockaddr_in));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = ::htons(SERVER_PORT);
	::inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);
	
	// 서버와 연결 시도, 최대 허용횟수를 넘어가면 에러 -> 강제종료
	int tryCount = 0;
	while (SOCKET_ERROR == ::connect(mSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in))) {
		if (tryCount >= MAX_TRY_CONNECT) {
			MessageBox(nullptr, L"최대 연결 시도횟수를 초과했습니다. 프로그램을 종료합니다.", L"서버와의 연결에 실패했습니다.", MB_OK | MB_ICONERROR);
			return false;
		}

		int result = MessageBox(nullptr, L"다시 시도할까요?", L"서버와의 연결에 실패했습니다.", MB_YESNO | MB_ICONERROR);
		if (IDNO == result) {
			return false;
		}

		tryCount += 1;
	}

	bool noDelay = true;
	::setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&noDelay), sizeof(noDelay));

	return true;
}

// 데이터 수신 쓰레드 생성
void ServerService::CreateRecvThread()
{
	mRecvThread = std::thread{ [=]()
		{
			// 추가로 할 작업이 있다면 여기에..
			Recv();
		}
	};
}

// 쓰레드 소멸 대기
void ServerService::Join()
{
	mRecvThreadRunning = false;
	::shutdown(mSocket, SD_BOTH);
	::closesocket(mSocket);

	if (mRecvThread.joinable()) {
		mRecvThread.join();
	}
}

// 실제로 데이터를 수신하는 함수
void ServerService::Recv()
{
	// 남는 데이터를 보관하기 위한 저장소
	char remainDataBuffer[RECV_SIZE]{ 0 };
	// 패킷 데이터 포인팅 -> 현재 처리중인 패킷의 주소
	char* currentData = mRecvBuffer;
	int remainSize = 0;

	while (mRecvThreadRunning) {
		// recv, 받은 데이터의 길이가 0또는 음수이면 종료 or 에러
		int len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);
		if (len <= 0) {
			break;
		}

		currentData = mRecvBuffer;

		if (mRemainByte > 0) {
			// 받은 recvBuffer에서 이전에 남았던 데이터를 앞에 붙이기 위한 작업
			memmove(mRecvBuffer + mRemainByte, mRecvBuffer, mRemainByte);
			memcpy(mRecvBuffer, currentData, mRemainByte);
			memset(mRecvBuffer, 0, RECV_SIZE);
		}

		remainSize = len + mRemainByte;

		while (remainSize > 0) {
			byte packetSize = currentData[0];
			if (packetSize > remainSize) {
				mRemainByte = remainSize;
				memcpy(remainDataBuffer, currentData, remainSize);
				break;
			}

			ProcessPacket(currentData);

			remainSize -= packetSize;
			currentData += packetSize;
		}
	}
}

// 받은 패킷 처리
void ServerService::ProcessPacket(char* packet)
{
	// PacketBase 구조체에서 1바이트는 size, 1바이트는 type으로 설정했음
	byte size = packet[0];
	byte type = packet[1];
	byte senderId = packet[2];

	switch (type) {
	case PACKET_PLAYER_CONNECT:
		{
			mId = senderId;
		}
		break;

	case PACKET_POSITION2D:
		{
			PacketPosition2D* position = reinterpret_cast<PacketPosition2D*>(packet);
			gGameFramework.UpdateJoinedPlayer(senderId, { position->x, position->y });
	}
		break;

	case PACKET_PLAYER_JOIN:
		{
			PacketPlayerJoin* joinedPlayer = reinterpret_cast<PacketPlayerJoin*>(packet);
			gGameFramework.JoinOtherPlayer(senderId, new Player{ joinedPlayer->x, joinedPlayer->y });
		}
		break;

	case PACKET_PLAYER_EXIT:
		{
			gGameFramework.ExitPlayer(senderId);
		}
		break;

	case PACKET_PING:
		{
			gGameFramework.PingResult(reinterpret_cast<PacketPing*>(packet)->timeSent);
		}
		break;

	default:
		break;
	}
}
