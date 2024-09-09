#pragma once

/* ----------------------------------------
*				TCPServerCore
* 
* �������� �� �⺻���� ����, ���ἳ��, �÷��̾� �߰�,
* ����� ������ Ŭ����
* 
* Ư���� ���� ���ٸ� ��ü�� �ϳ��� ����
*
  ---------------------------------------- */


class Client;

class TCPServerCore {
public:
	TCPServerCore();
	~TCPServerCore();

public:
	// �ʱ�ȭ, ������� �Ҹ���
	void Init();
	void Join();

	// Getter
	std::unordered_map<byte, Client>& GetClients();

	// Ŭ���̾�Ʈ ���� ���� ó��
	byte AddClient(SOCKET clientSocket);
	void ExitClient(byte id);

private:
	// ���ἳ���� �ʿ��� �Լ���
	void CreateCoreObjects();
	void StartAccept();

private:
	std::mutex mClientsLock{ };
	std::unique_ptr<class Listener> mListener;
	std::vector<std::thread> mClientServiceThreads;
	std::unordered_map<byte, Client> mClients;
};