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

void Client::ProcessPacket(char* packet)
{
	// PacketBase 구조체에서 1바이트는 size, 1바이트는 type으로 설정했음
	byte size = packet[0];
	byte type = packet[1];

	switch (type) {
	case PACKET_POSITION2D:
		for (auto& [id, client] : gServerCore.GetClients()) {
			client.Send(reinterpret_cast<PacketPosition2D*>(packet));
		}
		break;
	}
}
