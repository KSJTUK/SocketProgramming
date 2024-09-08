#include "pch.h"
#include "ServerService.h"
#include "GameFramework.h"
#include "Shape.h"

ServerService::ServerService()
{
	WSAData wsaData;
	ASSERT_CRASH(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "WSAStartup Failure");

	mSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_CRASH(INVALID_SOCKET != mSocket, "Socket is not Created");
}

ServerService::~ServerService()
{
	::shutdown(mSocket, SD_BOTH);
	::closesocket(mSocket);

	mRecvThread.join();
}

bool ServerService::ConnectToServer()
{
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(sockaddr_in));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = ::htons(SERVER_PORT);
	::inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);
	
	return SOCKET_ERROR != ::connect(mSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in));
}

void ServerService::Send(PacketBase* packet)
{
	::send(
		mSocket,
		reinterpret_cast<char*>(packet),
		static_cast<int>(packet->size),
		0
	);
}

void ServerService::CreateRecvThread()
{
	mRecvThread = std::thread{ [=]()
		{
			Recv();
		}
	};
}

void ServerService::Recv()
{
	while (true) {
		int len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);

		if (len <= 0) {
			::shutdown(mSocket, SD_BOTH);
			::closesocket(mSocket);
			break;
		}

		ProcessPacket(mRecvBuffer);
	}
}

void ServerService::ProcessPacket(char* packet)
{
	// PacketBase 구조체에서 1바이트는 size, 1바이트는 type으로 설정했음
	byte size = packet[0];
	byte type = packet[1];

	switch (type) {
	case PACKET_POSITION2D:
		{
			PacketPosition2D* position = reinterpret_cast<PacketPosition2D*>(packet);
			gGameFramework.AddShape(new PointShape{ position->x, position->y, gGameFramework.GetDrawBuffer() });
		}
		break;
	}
}
