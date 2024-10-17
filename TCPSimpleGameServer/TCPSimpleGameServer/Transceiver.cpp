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
	// ���� �����͸� �����ϱ� ���� �����
	char packetBuffer[RECV_SIZE]{ 0 };
	// ��Ŷ ������ ������ -> ���� ó������ ��Ŷ�� �ּ�
	char* currentData = mRecvBuffer;
	int remainSize = 0;
	int prevRemain = 0;
	int len = 0;

	while (true) {
		// recv, ���� �������� ���̰� 0�Ǵ� �����̸� ���� or ����
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
