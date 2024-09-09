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


class Client;

class TCPServerCore {
public:
	TCPServerCore();
	~TCPServerCore();

public:
	// 초기화, 쓰레드들 소멸대기
	void Init();
	void Join();

	// Getter
	std::unordered_map<byte, Client>& GetClients();

	// 클라이언트 입장 퇴장 처리
	byte AddClient(SOCKET clientSocket);
	void ExitClient(byte id);

private:
	// 연결설정에 필요한 함수들
	void CreateCoreObjects();
	void StartAccept();

private:
	std::mutex mClientsLock{ };
	std::unique_ptr<class Listener> mListener;
	std::vector<std::thread> mClientServiceThreads;
	std::unordered_map<byte, Client> mClients;
};