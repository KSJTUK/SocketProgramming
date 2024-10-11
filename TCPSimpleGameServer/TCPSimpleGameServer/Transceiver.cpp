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
			if (len + prevRemain > 1024) {
				std::cout << "�������� ũ�Ⱑ ������ ũ�⸦ �ѽ��ϴ�." << std::endl;
				break;
			}

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

			gGameServer.ProcessPacket(currentData);

			remainSize -= packetSize;
			currentData += packetSize;
		}
	}
}
