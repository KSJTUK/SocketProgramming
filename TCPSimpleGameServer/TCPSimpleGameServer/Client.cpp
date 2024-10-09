#include "pch.h"
#include "Client.h"
#include "ClientSession.h"

/* ----------------------------------------
*
*				Client
*
  ---------------------------------------- */


Client::Client()
	: mSocket{ INVALID_SOCKET },
	mHostInfo{ },
	mSession{ std::make_unique<ClientSession>() }
{
}

Client::~Client()
{
}

const byte Client::GetId() const
{
	return mSession->GetId();
}

std::pair<float, float> Client::GetPosition() const
{
	return mSession->GetPosition();
}

void Client::Join(SOCKET clientSocket, byte id)
{
	mSocket = clientSocket;
	mSession->SetId(id);
	mClientState = CLIENT_STATE::JOINED;
}

void Client::Exit()
{
	mSession->SetId(NULL_CLIENT_ID);
	mClientState = CLIENT_STATE::EXITED;
}

void Client::CloseSocket()
{
	linger ln{ 1, 0 };
	::setsockopt(mSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&ln), sizeof(ln));

	::shutdown(mSocket, SD_BOTH);
	::closesocket(mSocket);
}

void Client::Recv()
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

void Client::ProcessPacket(char* packet)
{
	// PacketBase ����ü���� 1����Ʈ�� size, 1����Ʈ�� type, 1����Ʈ�� �۽��� id�� ��������
	byte size = packet[0];
	byte type = packet[1];
	byte senderId = packet[2];

	switch (type) {
	case PACKET_PLAYER_CONNECT:
		break;

	case PACKET_POSITION2D:
		gServerCore.Broadcast<PacketPosition2D>(PACKET_POSITION2D, senderId, packet);
		break;

	case PACKET_PLAYER_JOIN:
	{
		// ���� ������Ʈ
		PacketPlayerJoin* joinPacket = reinterpret_cast<PacketPlayerJoin*>(packet);
		mSession->SetPosition(joinPacket->x, joinPacket->y);
	}

	gServerCore.SendOtherClientsSession(senderId);
	gServerCore.Broadcast<PacketPlayerJoin>(PACKET_PLAYER_JOIN, senderId, packet);
	break;

	case PACKET_PLAYER_EXIT:
		gServerCore.Broadcast<PacketPlayerExit>(PACKET_PLAYER_EXIT, senderId, packet);
		gServerCore.ExitClient(senderId);
		break;

	case PACKET_PING:
		Send<PacketPing>(PACKET_PING, packet);
		break;
	}
}