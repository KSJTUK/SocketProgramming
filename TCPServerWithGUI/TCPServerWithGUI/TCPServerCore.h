#pragma once

/* ----------------------------------------
*				TCPServerCore
* 
* 서버에서 할 기본적인 연산, 연결설정, 플레이어 추가,
* 등등을 수행할 클래스
* 
* 특별한 일이 없다면 객체는 하나만 생성
*
  ---------------------------------------- */

#include "Client.h"

class TCPServerCore {
public:
	TCPServerCore();
	~TCPServerCore();

public:
	/** 초기화 **/
	void Init();
	/** 쓰레드들 소멸대기 **/
	void Join();

	/** Getter **/
	std::vector<Client>& GetClients();

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
			if (mClients[id].GetState() == CLIENT_STATE::EXITED) {
				continue;
			}
			mClients[id].Send(&packet);
		}
	}

private:
	/* 연결설정에 필요한 함수들 */
	void CreateCoreObjects();
	void StartAccept();

private:
	std::unique_ptr<class Listener> mListener;
	std::vector<std::thread> mClientServiceThreads;

	// 클라이언트의 개수를 고정해둔다면 굳이 map을 쓸 필요가 있는가?
	// - 24.09.23 std::vector로 변경
	/* 공유 변수 Vector */
	std::vector<Client> mClients;
	// 자료 구조의 변경을 보호하기 위한 mutex
};