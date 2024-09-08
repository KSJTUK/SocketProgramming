#include "pch.h"
#include "Client.h"

Client::Client() = default;

Client::Client(unsigned short cid, SOCKET socket, const Address::NetHostInfo& hostInfo)
	: mClientId{ cid },
	mSocket{ socket },
	mHostInfo{ hostInfo }
{
}

Client::~Client()
{
}

void Client::Send(PacketBase* packet)
{
	::send(
		mSocket,
		reinterpret_cast<char*>(packet),
		static_cast<int>(packet->size),
		0
	);
}

void Client::Recv()
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

void Client::BroadCasePacket(byte senderId, PacketBase* packet)
{
	for (auto& [id, client] : gServerCore.GetClients()) {
		if (id == senderId) {
			continue;
		}

		client.Send(reinterpret_cast<PacketPosition2D*>(packet));
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
		BroadCasePacket(senderId, reinterpret_cast<PacketPosition2D*>(packet));
		break;

	case PACKET_PLAYER_JOIN:
		BroadCasePacket(senderId, reinterpret_cast<PacketPlayerJoin*>(packet));
		break;

	case PACKET_PLAYER_EXIT:
		BroadCasePacket(senderId, reinterpret_cast<PacketPlayerExit*>(packet));
		break;

	case PACKET_MOVE2D:
		BroadCasePacket(senderId, reinterpret_cast<PacketMove2D*>(packet));
		break;
	}
}
