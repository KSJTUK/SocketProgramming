#include "pch.h"
#include "Transceiver.h"

Transceiver::Transceiver()
	: mSocket{ INVALID_SOCKET },
	mHostInfo{ }
{
}

Transceiver::~Transceiver()
{
}

void Transceiver::Init(SOCKET socket)
{
	mSocket = socket;
}

void Transceiver::CloseSocket()
{
	linger ln{ 1, 0 };
	::setsockopt(mSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&ln), sizeof(ln));

	::shutdown(mSocket, SD_BOTH);
	::closesocket(mSocket);
}

void Transceiver::Recv()
{
	// ���� �����͸� �����ϱ� ���� �����
	char remainDataBuffer[RECV_SIZE]{ 0 };
	// ��Ŷ ������ ������ -> ���� ó������ ��Ŷ�� �ּ�
	char* currentData = mRecvBuffer;
	int remainSize = 0;
	int prevRemain = 0;

	while (true) {
		// recv, ���� �������� ���̰� 0�Ǵ� �����̸� ���� or ����
		int len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);
		if (len <= 0) {
			break;
		}

		if (prevRemain > 0) {
			// ���� recvBuffer���� ������ ���Ҵ� �����͸� �տ� ���̱� ���� �۾�
			memmove(mRecvBuffer + prevRemain, mRecvBuffer, len);
			memcpy(mRecvBuffer, remainDataBuffer, prevRemain);
			memset(remainDataBuffer, 0, RECV_SIZE);
		}

		currentData = mRecvBuffer;
		remainSize = len + prevRemain;
		prevRemain = 0;

		while (remainSize > 0) {
			byte packetSize = currentData[0];
			if (packetSize > remainSize) {
				prevRemain = remainSize;
				memcpy(remainDataBuffer, currentData, remainSize);
				break;
			}

			ProcessPacket(currentData);

			remainSize -= packetSize;
			currentData += packetSize;
		}
	}
}

void Transceiver::ProcessPacket(char* packet)
{
	// PacketBase ����ü���� 1����Ʈ�� size, 1����Ʈ�� type, 1����Ʈ�� �۽��� id�� ��������
	byte size = packet[0];
	byte type = packet[1];
	byte senderId = packet[2];

	switch (type) {
	case PACKET_PLAYER_CONNECT:
		break;

	case PACKET_POSITION2D:
		gGameServer.Broadcast<PacketPosition2D>(PACKET_POSITION2D, senderId, packet);
		break;

	case PACKET_PLAYER_INPUT:
		break;

	case PACKET_PLAYER_JOIN:
		gGameServer.UpdatePlayer(senderId, reinterpret_cast<PacketPlayerJoin*>(packet)->pos);
		gGameServer.SendOtherClientsSession(senderId);
		gGameServer.Broadcast<PacketPlayerJoin>(PACKET_PLAYER_JOIN, senderId, packet);
		break;

	case PACKET_PLAYER_EXIT:
		gGameServer.Broadcast<PacketPlayerExit>(PACKET_PLAYER_EXIT, senderId, packet);
		gGameServer.ExitClient(senderId);
		break;

	case PACKET_PING:
		Send<PacketPing>(PACKET_PING, senderId, packet);
		break;
	}
}
