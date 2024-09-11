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

// ������ ���ἳ��
bool ServerService::ConnectToServer()
{
	// ���� IP, PORT��ȣ ����
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(sockaddr_in));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = ::htons(SERVER_PORT);
	::inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);
	
	// ������ ���� �õ�, �ִ� ���Ƚ���� �Ѿ�� ���� -> ��������
	int tryCount = 0;
	while (SOCKET_ERROR == ::connect(mSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in))) {
		if (tryCount >= MAX_TRY_CONNECT) {
			MessageBox(nullptr, L"�ִ� ���� �õ�Ƚ���� �ʰ��߽��ϴ�. ���α׷��� �����մϴ�.", L"�������� ���ῡ �����߽��ϴ�.", MB_OK | MB_ICONERROR);
			return false;
		}

		int result = MessageBox(nullptr, L"�ٽ� �õ��ұ��?", L"�������� ���ῡ �����߽��ϴ�.", MB_YESNO | MB_ICONERROR);
		if (IDNO == result) {
			return false;
		}

		tryCount += 1;
	}

	return true;
}

// ������ ���� ������ ����
void ServerService::CreateRecvThread()
{
	mRecvThread = std::thread{ [=]()
		{
			// �߰��� �� �۾��� �ִٸ� ���⿡..
			Recv();
		}
	};
}

// ������ �Ҹ� ���
void ServerService::Join()
{
	mRecvThreadRunning = false;
	::shutdown(mSocket, SD_BOTH);
	::closesocket(mSocket);

	if (mRecvThread.joinable()) {
		mRecvThread.join();
	}
}

// ������ �����͸� �����ϴ� �Լ�
void ServerService::Recv()
{
	while (mRecvThreadRunning) {
		int len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);

		if (len <= 0) {
			break;
		}

		ProcessPacket(mRecvBuffer);
	}
}

// ���� ��Ŷ ó��
void ServerService::ProcessPacket(char* packet)
{
	// PacketBase ����ü���� 1����Ʈ�� size, 1����Ʈ�� type���� ��������
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

	case PACKET_PING:
		{
			gGameFramework.PingResult(reinterpret_cast<PacketPing*>(packet)->timeSent);
		}
		break;

	default:
		break;
	}
}
