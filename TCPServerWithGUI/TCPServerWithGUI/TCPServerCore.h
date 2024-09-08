#pragma once

class TCPServerCore {
public:
	TCPServerCore();
	~TCPServerCore();

public:
	void Init();
	void Join();

private:
	void CreateCoreObjects();
	void StartAccept();
	void RecvAndEcho(SOCKET clientSocket);

private:
	std::unique_ptr<class Listener> mListener;
	std::vector<std::thread> mClientServiceThreads;
};