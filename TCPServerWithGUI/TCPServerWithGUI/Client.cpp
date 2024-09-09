#include "pch.h"
#include "Client.h"
#include "ClientSession.h"

/* ----------------------------------------
*
*				Client
*
  ---------------------------------------- */


Client::Client() = default;

Client::Client(byte cid, SOCKET socket, const Address::NetHostInfo& hostInfo)
	: mSocket{ socket },
	mHostInfo{ hostInfo }
{
	mSession = std::make_unique<ClientSession>(cid);
}

Client::Client(Client&& other) noexcept
	: mSocket{ other.mSocket },
	mHostInfo{ std::move(other.mHostInfo) }
{
	mSession.reset(other.mSession.release());
}

Client::~Client()
{
}

const byte Client::GetId() const
{
	return mSession->GetId();
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

void Client::ProcessPacket(char* packet)
{
	// PacketBase 구조체에서 1바이트는 size, 1바이트는 type, 1바이트는 송신자 id로 설정했음
	byte size = packet[0];
	byte type = packet[1];
	byte senderId = packet[2];

	switch (type) {
	case PACKET_PLAYER_CONNECT:
		break;

	case PACKET_POSITION2D:
		BroadCasePacket<PacketPosition2D>(PACKET_POSITION2D, senderId, packet);
		break;

	case PACKET_PLAYER_JOIN:
		BroadCasePacket<PacketPlayerJoin>(PACKET_PLAYER_JOIN, senderId, packet);
		break;

	case PACKET_PLAYER_EXIT:
		BroadCasePacket<PacketPlayerExit>(PACKET_PLAYER_EXIT, senderId, packet);
		break;

	case PACKET_MOVE2D:
		BroadCasePacket<PacketMove2D>(PACKET_MOVE2D, senderId, packet);
		break;
	}
}
