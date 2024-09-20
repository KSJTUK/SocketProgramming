#include "pch.h"
#include "Listener.h"

/*----------------------------------
*
*			Listener
*
----------------------------------*/

Listener::Listener(const char* ip, unsigned short port)
	: mListenSocket{ ::socket(AF_INET, SOCK_STREAM, 0) }
{
	ASSERT(INVALID_SOCKET != mListenSocket, "ListenSocket Create Failure");

	mServerAddress.sin_family = AF_INET;
	mServerAddress.sin_port = ::htons(port);
	mServerAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);

	auto bindResult = ::bind(mListenSocket, reinterpret_cast<sockaddr*>(&mServerAddress), sizeof(sockaddr_in));
	ASSERT(SOCKET_ERROR != bindResult, "Bind Function Failure");

	auto listenResult = ::listen(mListenSocket, SOMAXCONN);
	ASSERT(SOCKET_ERROR != listenResult, "Listen Function Failure");
}

Listener::~Listener()
{
	::closesocket(mListenSocket);
}

SOCKET Listener::Accept()
{
	return ::accept(mListenSocket, nullptr, nullptr);
}