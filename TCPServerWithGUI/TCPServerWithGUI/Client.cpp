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

Client::Client(Client&& other) noexcept
	: mSocket{ other.mSocket },
	mHostInfo{ std::move(other.mHostInfo) }
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
}

void Client::Exit()
{
	mSession->SetId(NULL_CLIENT_ID);
	mClientState = CLIENT_STATE::EXITED;
	::closesocket(mSocket);
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

#if NETWORK_DEBUG
		//{
		//	std::lock_guard ioGuard{ mIOLock };
		//	if (len != (int)mRecvBuffer[0]) {
		//		std::cout << "PACKET Type: " << gPacketTypeStrs[mRecvBuffer[1]] << std::endl;;
		//		std::cout << "Send Byte: " << (int)mRecvBuffer[0] << ", Recv Byte: " << len << std::endl;
		//	}
		//}

		std::cout << "On Recv: " << len << "\n";
#endif

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
		gServerCore.Broadcast<PacketPosition2D>(PACKET_POSITION2D, senderId, packet);
		break;

	case PACKET_PLAYER_JOIN:
		{
			// 세션 업데이트
			PacketPlayerJoin* joinPacket = reinterpret_cast<PacketPlayerJoin*>(packet);
			mSession->SetPosition(joinPacket->x, joinPacket->y);
			mClientState = CLIENT_STATE::JOINED;
		}

		gServerCore.Broadcast<PacketPlayerJoin>(PACKET_PLAYER_JOIN, senderId, packet);
		gServerCore.SendOtherClientsSession(senderId);
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