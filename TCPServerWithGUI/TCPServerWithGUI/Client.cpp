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
	mHostInfo{ std::move(other.mHostInfo) },
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
	linger ln{ 0, 0 };
	::setsockopt(mSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&ln), sizeof(ln));

	::shutdown(mSocket, SD_BOTH);
	::closesocket(mSocket);
}

void Client::Recv()
{
	// 남는 데이터를 보관하기 위한 저장소
	char remainDataBuffer[RECV_SIZE]{ 0 };
	// 패킷 데이터 포인팅 -> 현재 처리중인 패킷의 주소
	char* currentData = mRecvBuffer;
	int remainSize = 0;

	while (true) {
		// recv, 받은 데이터의 길이가 0또는 음수이면 종료 or 에러
		int len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);
		if (len <= 0) {
			::shutdown(mSocket, SD_BOTH);
			::closesocket(mSocket);
			break;
		}

		gIOLock.lock();
		std::cout << "수신 바이트: " << len << "\n";
		gIOLock.unlock();

		currentData = mRecvBuffer;

		if (mRemainByte > 0) {
			// 받은 recvBuffer에서 이전에 남았던 데이터를 앞에 붙이기 위한 작업
			memmove(mRecvBuffer + mRemainByte, mRecvBuffer, mRemainByte);
			memcpy(mRecvBuffer, remainDataBuffer, mRemainByte);

			memset(mRecvBuffer, 0, RECV_SIZE);
			memset(remainDataBuffer, 0, RECV_SIZE);
		}

		remainSize = len + mRemainByte;

		// 패킷이 뭉쳐서 오면?
		// 패킷의 사이즈는 이미 적어 두었음. 첫번째 바이트가 패킷의 사이즈이므로,
		// 이 사이즈만큼 포인터를 이동시키면서 남지 않을때까지 처리
		// 패킷이 다 오지 않은 경우? -> 받은 사이즈 및 남은 데이터를 저장하고 다시 recv
		while (remainSize > 0) {
			byte packetSize = currentData[0];
			if (packetSize > remainSize) {
				mRemainByte = remainSize;
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
		}

		gServerCore.SendOtherClientsSession(senderId);
		gServerCore.Broadcast<PacketPlayerJoin>(PACKET_PLAYER_JOIN, senderId, packet);
		break;

	case PACKET_PLAYER_EXIT:
		gServerCore.Broadcast<PacketPlayerExit>(PACKET_PLAYER_EXIT, senderId, packet);
		CloseSocket();
		break;

	case PACKET_PING:
		Send<PacketPing>(PACKET_PING, packet);
		break;
	}
}