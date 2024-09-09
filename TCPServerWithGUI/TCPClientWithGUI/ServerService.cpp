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
	ASSERT_CRASH(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "WSAStartup Failure");

	mSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_CRASH(INVALID_SOCKET != mSocket, "Socket is not Created");
}

ServerService::~ServerService() = default;

// 서버와 연결설정
bool ServerService::ConnectToServer()
{
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(sockaddr_in));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = ::htons(SERVER_PORT);
	::inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);
	
	return SOCKET_ERROR != ::connect(mSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in));
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

	mRecvThread.join();
}

// 실제로 데이터를 수신하는 함수
void ServerService::Recv()
{
	while (mRecvThreadRunning) {
		int len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);

		if (len <= 0) {
			::shutdown(mSocket, SD_BOTH);
			::closesocket(mSocket);
			break;
		}

		ProcessPacket(mRecvBuffer);
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
		gGameFramework.AddShape(new PointShape{ position->x, position->y, gGameFramework.GetDrawBuffer() });
	}
	break;

	case PACKET_PLAYER_JOIN:
	{
		PacketPlayerJoin* joinedPlayer = reinterpret_cast<PacketPlayerJoin*>(packet);
		gGameFramework.JoinOtherPlayer(senderId, new Player{ joinedPlayer->x, joinedPlayer->y });
	}
	break;

	case PACKET_MOVE2D:
	{
		PacketMove2D* moveInfo = reinterpret_cast<PacketMove2D*>(packet);
		gGameFramework.UpdateJoinedPlayer(senderId, { moveInfo->x, moveInfo->y }, moveInfo->velocity);
	}
	break;

	case PACKET_PLAYER_EXIT:
	{
		gGameFramework.ExitPlayer(senderId);
	}
	break;
	}
}
