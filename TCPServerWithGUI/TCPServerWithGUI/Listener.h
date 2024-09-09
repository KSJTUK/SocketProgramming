#pragma once

/*----------------------------------			
*			Listener
* 
* 클라이언트 연결설정을 수행하는 클래스
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