#include "pch.h"
#include "Transceiver.h"

/* ----------------------------------------
*
*				Transceiver
*
  ---------------------------------------- */

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
	// 남는 데이터를 보관하기 위한 저장소
	char remainDataBuffer[RECV_SIZE]{ 0 };
	// 패킷 데이터 포인팅 -> 현재 처리중인 패킷의 주소
	char* currentData = mRecvBuffer;
	int remainSize = 0;
	int prevRemain = 0;

	while (true) {
		// recv, 받은 데이터의 길이가 0또는 음수이면 종료 or 에러
		int len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);
		if (len <= 0) {
			break;
		}

		if (prevRemain > 0) {
			if (len + prevRemain > 1024) {
				std::cout << "데이터의 크기가 버퍼의 크기를 넘습니다." << std::endl;
				break;
			}

			// 받은 recvBuffer에서 이전에 남았던 데이터를 앞에 붙이기 위한 작업
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

			gGameServer.ProcessPacket(currentData);

			remainSize -= packetSize;
			currentData += packetSize;
		}
	}
}
