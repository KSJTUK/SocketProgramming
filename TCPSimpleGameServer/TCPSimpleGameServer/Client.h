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
* 실제로 연결된 주체를 표현할 클래스 : Player
* 
* 10-10 수정 : 
* Transceiver 객체가 데이터 송수신을 담당하도록 변경
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
