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
	char packetBuffer[RECV_SIZE]{ 0 };
	// 패킷 데이터 포인팅 -> 현재 처리중인 패킷의 주소
	char* currentData = mRecvBuffer;
	int remainSize = 0;
	int prevRemain = 0;
	int len = 0;

	while (true) {
		// recv, 받은 데이터의 길이가 0또는 음수이면 종료 or 에러
		len = ::recv(mSocket, mRecvBuffer, RECV_SIZE, 0);
		if (len == 0) {
			break;
		}
		else if (len < 0) {
#ifdef _DEBUG || DEBUG
			MessageBoxA(nullptr, GetErrorString().c_str(), "Recv Error", MB_OK | MB_ICONERROR);
#endif
			break;
		}

		remainSize = len;
		currentData = mRecvBuffer;

		if (prevRemain > 0) {
			int cpySize = packetBuffer[0] - prevRemain;
			memcpy(packetBuffer + prevRemain, mRecvBuffer, cpySize);
			currentData += cpySize;
			remainSize -= cpySize;

			gGameServer.ProcessPacket(packetBuffer);
			memset(packetBuffer, 0, cpySize + prevRemain);

			prevRemain = 0;
		}

		while (remainSize > 0) {
			byte packetSize = currentData[0];
			if (packetSize > remainSize) {
				prevRemain = remainSize;
				memcpy(packetBuffer, currentData, remainSize);
				break;
			}

			memcpy(packetBuffer, currentData, packetSize);
			gGameServer.ProcessPacket(packetBuffer);
			memset(packetBuffer, 0, packetSize);

			remainSize -= packetSize;
			currentData += packetSize;
		}
	}
}
