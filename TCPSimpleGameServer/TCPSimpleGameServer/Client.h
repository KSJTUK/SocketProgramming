#pragma once

#include "Transceiver.h"

enum class CLIENT_STATE : char {
	JOINED,
	EXITED,
};

/* ----------------------------------------
*
*				Client
*
* ������ ����� ��ü�� ǥ���� Ŭ���� : Player
* 
* 10-10 ���� : 
* Transceiver ��ü�� ������ �ۼ����� ����ϵ��� ����
* 
  ---------------------------------------- */

class Client {
public:
	Client();
	~Client();

public:
	Transceiver& GetTransceiver();
	const byte GetId() const;
	Position GetPosition() const;
	CLIENT_STATE GetState() const { return mClientState; }

	std::mutex& GetMutex() { return mStateLock; }

	void SetPosition(const Position pos);

public:
	void Join(SOCKET clientSocket, byte id);
	void Exit();

private:
	std::mutex mStateLock;

	Transceiver mTransceiver;

	Position mPosition{ };
	CLIENT_STATE mClientState{ CLIENT_STATE::EXITED };
	byte mId{ NULL_CLIENT_ID };
};
