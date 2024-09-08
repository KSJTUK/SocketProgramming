#pragma once

/*----------------------------------
*			
*			Listener
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