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

	// ���̱� �˰��� ��� ����
	bool noDelay = true;
	::setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&noDelay), sizeof(noDelay));

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

	// Graceful Shutdown
	linger ln{ 1, 0 };
	::setsockopt(mSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&ln), sizeof(ln));

	::shutdown(mSocket, SD_BOTH);
	::closesocket(mSocket);

	if (mRecvThread.joinable()) {
		mRecvThread.join();
	}
}

// ������ �����͸� �����ϴ� �Լ�
void ServerService::Recv()
{
	// ���� �����͸� �����ϱ� ���� �����
	char packetBuffer[RECV_SIZE]{ 0 };
	// ��Ŷ ������ ������ -> ���� ó������ ��Ŷ�� �ּ�
	char* currentData = mRecvBuffer;
	int remainSize = 0;
	int prevRemain = 0;
	int len = 0;

	while (true) {
		// recv, ���� �������� ���̰� 0�Ǵ� �����̸� ���� or ����
		len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);
		if (len == 0) {
			break;
		}
		else if (len < 0) {
#ifdef _DEBUG || DEBUG
			MessageBoxA(nullptr, GetErrorString().c_str(), "Recv Error", MB_OK | MB_ICONERROR);
#endif
			break;
		}

		remainSize = len;
		currentData = mRecvBuffer;

		if (prevRemain > 0) {
			int cpySize = packetBuffer[0] - prevRemain;
			memcpy(packetBuffer + prevRemain, mRecvBuffer, cpySize);
			currentData += cpySize;
			remainSize -= cpySize;
			ProcessPacket(packetBuffer);
			memset(packetBuffer, 0, cpySize + prevRemain);
			prevRemain = 0;
		}

		while (remainSize > 0) {
			byte packetSize = currentData[0];
			if (packetSize > remainSize) {
				prevRemain = remainSize;
				memcpy(packetBuffer, currentData, remainSize);
				break;
			}

			memcpy(packetBuffer, currentData, packetSize);
			ProcessPacket(packetBuffer);
			memset(packetBuffer, 0, packetSize);

			remainSize -= packetSize;
			currentData += packetSize;
		}
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
		mId = senderId;
		break;

	case PACKET_POSITION2D:
	{
		PacketPosition2D* position = reinterpret_cast<PacketPosition2D*>(packet);
		gGameFramework.UpdateJoinedPlayer(senderId, position->pos);
	}
	break;

	case PACKET_PLAYER_JOIN:
		if (senderId == mId) break;
		else {
		PacketPlayerJoin* playerJoin = reinterpret_cast<PacketPlayerJoin*>(packet);
		gGameFramework.JoinOtherPlayer(senderId, playerJoin->pos);
		}
	break;

	case PACKET_PLAYER_EXIT:
		if (senderId == mId) break;
		gGameFramework.ExitPlayer(senderId);
		break;

	case PACKET_PING:
		gGameFramework.PingResult(reinterpret_cast<PacketPing*>(packet)->timeSent);
		break;

	case PACKET_OBJECT_INFO:
		{
			PacketObjectInfo* objectInfo = reinterpret_cast<PacketObjectInfo*>(packet);
			gGameFramework.UpdateObject(objectInfo);
		}
		break;

	default:
		break;
	}
}
