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
	bool nodelay = true;
	::setsockopt(mSocket, SOL_SOCKET, TCP_NODELAY, reinterpret_cast<const char*>(&nodelay), 1);
	mSession = std::make_unique<ClientSession>(cid);
}

Client::Client(Client&& other) noexcept
	: mSocket{ other.mSocket },
	mHostInfo{ std::move(other.mHostInfo) }
{
	bool nodelay = true;
	::setsockopt(mSocket, SOL_SOCKET, TCP_NODELAY, reinterpret_cast<const char*>(&nodelay), 1);
	mSession.reset(other.mSession.release());
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
		{
			// 세션 업데이트
			PacketPlayerJoin* joinPacket = reinterpret_cast<PacketPlayerJoin*>(packet);
			mSession->SetPosition(joinPacket->x, joinPacket->y);
		}

		BroadCasePacket<PacketPlayerJoin>(PACKET_PLAYER_JOIN, senderId, packet);
		SendOtherClientsSession(senderId);
		break;

	case PACKET_PLAYER_EXIT:
		BroadCasePacket<PacketPlayerExit>(PACKET_PLAYER_EXIT, senderId, packet);
		break;

	case PACKET_PING:
		Send<PacketPing>(PACKET_PING, packet);
		break;
	}
}

void Client::SendOtherClientsSession(byte targetId)
{
	PacketPlayerJoin packet{ sizeof(PacketPlayerJoin), PACKET_PLAYER_JOIN, targetId };

	// 타겟으로 하는 클라이언트가 존재하는지 여부 검사
	auto& clients = gServerCore.GetClients();
	if (not clients.contains(targetId)) {
		return;
	}

	// 존재한다면 그 클라이언트에게 다른 클라이언트들의 존재여부 송신
	Client& targetClient = clients[targetId];

	for (auto& [id, client] : clients) {
		if (id == targetId) {
			continue;
		}

		// 다른 클라이언트의 위치정보를 받아 송신
		auto [x, y] = client.GetPosition();
		packet.x = x;
		packet.y = y;
		packet.senderId = id;

		targetClient.Send(&packet);
	}
}
