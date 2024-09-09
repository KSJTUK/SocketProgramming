#pragma once

class Client;

class TCPServerCore {
public:
	TCPServerCore();
	~TCPServerCore();

public:
	void Init();
	void Join();

	std::unordered_map<byte, Client>& GetClients();

	byte AddClient(SOCKET clientSocket);
	void ExitClient(byte id);

private:
	void CreateCoreObjects();
	void StartAccept();

private:
	std::mutex mClientsLock{ };
	std::unique_ptr<class Listener> mListener;
	std::vector<std::thread> mClientServiceThreads;
	std::unordered_map<byte, Client> mClients;
};