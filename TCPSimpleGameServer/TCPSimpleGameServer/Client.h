#pragma once

#include "Transceiver.h"
#include "ProcessKeyInput.h"

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
	ProcessKeyInput& GetInputProcessor();
	const byte GetId() const;
	Vec2D GetPosition() const;
	CLIENT_STATE GetState() const { return mClientState; }

	std::mutex& GetMutex() { return mStateLock; }

	void SetPosition(const Vec2D pos);

public:
	void Join(SOCKET clientSocket, byte id);
	void Exit();

	// 
	void MoveLeft(float deltaTime);
	void MoveUp(float deltaTime);
	void MoveRight(float deltaTime);
	void MoveDown(float deltaTime);

	void Update(const float deltaTime);

	bool CheckCollision(class Object* const obj);
	void HandleCollision(class Object* const obj);

private:
	std::mutex mStateLock;

	Transceiver mTransceiver;
	ProcessKeyInput mInputProcessor;

	CLIENT_STATE mClientState{ CLIENT_STATE::EXITED };
	byte mId{ NULL_CLIENT_ID };

	int mMoveDirIndex;
	std::unique_ptr<class Collider> mCollider;
};
