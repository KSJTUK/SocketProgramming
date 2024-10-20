#pragma once

/* ----------------------------------------
*				GameServer
*
* 서버에서 할 기본적인 연산, 연결설정, 플레이어 추가,
* 등등을 수행할 클래스
*
* 특별한 일이 없다면 객체는 하나만 생성
*
  ---------------------------------------- */

#include "Client.h"

class Object;

class GameServer {
public:
	GameServer();
	~GameServer();

public:
	/** 초기화 **/
	void Init();
	/** 쓰레드들 소멸대기 **/
	void Join();

	/** Getter **/
	std::vector<std::shared_ptr<Client>>& GetClients();

	/** 클라이언트 입장 퇴장 처리 **/
	byte AddClient(SOCKET clientSocket);
	void ExitClient(byte id);

	/** 새로 들어온 플레이어에게 기존 플레이어의 정보를 보냄 **/
	void SendOtherClientsSession(byte targetId);

	/* 모든 클라이언트에게 같은 패킷을 보냄 */
	template <typename PacketType> requires std::is_base_of_v<PacketBase, PacketType>
	void Broadcast(byte type, byte senderId, char* data)
	{
		PacketType packet;
		std::memcpy(&packet, data, sizeof(PacketType));
		packet.size = sizeof(PacketType);
		packet.type = type;
		packet.senderId = senderId;

		for (byte id = 0; id < MAX_CLIENT; ++id) {
			if (senderId == id) {
				mClients[id]->GetTransceiver().Send(&packet);
				continue;
			}

			std::lock_guard stateLock{ mClients[id]->GetMutex() };
			if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
				mClients[id]->GetTransceiver().Send(&packet);
			}
		}
	}

	void ProcessPacket(char* packet);

	void SendClientsInfo();
	void SendObjectsInfo();
	Object* CreateObject(OBJECT_TYPE objType, Vec2D pos, SizeF size, DWORD color = RGB(255, 255, 255));
	bool AllocObject(OBJECT_TYPE objType, Vec2D pos, SizeF size, DWORD color=RGB(255, 255, 255));

public:
	/* 게임 월드 작업 관련 함수들 */
	void Update();

private:
	/* 연결설정에 필요한 함수들 */
	void CreateCoreObjects();
	void Accept();

public:
	inline static std::atomic<float> mDeltaTime{ };

private:
	std::unique_ptr<class Listener> mListener;
	std::unique_ptr<class Timer> mTimer;

	std::mutex mBroadcastLock;

	std::thread mAcceptThread;
	std::vector<std::thread> mClientServiceThreads;

	std::vector<std::shared_ptr<Client>> mClients;
	std::array<bool, MAX_OBJECT> mObjectAlive{ };
	std::vector<std::unique_ptr<Object>> mObjects;
};