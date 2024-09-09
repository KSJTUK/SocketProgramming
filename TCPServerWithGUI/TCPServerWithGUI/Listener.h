#pragma once

/*----------------------------------			
*			Listener
* 
* Ŭ���̾�Ʈ ���ἳ���� �����ϴ� Ŭ����
* 
----------------------------------*/

class Listener {
public:
	explicit Listener(const char* ip, unsigned short port);
	~Listener();

public:
	SOCKET Accept();

private:
	sockaddr_in mServerAddress{ };
	SOCKET mListenSocket{ };
};