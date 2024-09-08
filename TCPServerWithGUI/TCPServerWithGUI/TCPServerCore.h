#pragma once

class Client;

class TCPServerCore {
public:
	TCPServerCore();
	~TCPServerCore();

public:
	void Init();
	void Join();

	std::unordered_map<unsigned short, Client>& GetClients();

	short AddClient(SOCKET clientSocket);
	void ExitClient(unsigned short id);

private:
	void CreateCoreObjects();
	void StartAccept();

private:
	std::mutex mClientsLock{ };
	std::unique_ptr<class Listener> mListener;
	std::vector<std::thread> mClientServiceThreads;
	std::unordered_map<unsigned short, Client> mClients;
};