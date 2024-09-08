#pragma once

#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <list>
#include <vector>
#include <memory>

#include <thread>
#include <mutex>
#include <atomic>

#include "Macros.h"
#include "Random.h"
#include "Protocol.h"

namespace Address {
	struct NetHostInfo {
		std::string ip;
		unsigned short port;
	};

	inline NetHostInfo GetHostInfo(SOCKET socket)
	{
		NetHostInfo hostInfo{ };
		hostInfo.ip.reserve(INET_ADDRSTRLEN);

		sockaddr_in address;
		int addressLength{ sizeof(sockaddr_in) };
		::getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressLength);

		::inet_ntop(AF_INET, &address.sin_addr, hostInfo.ip.data(), INET_ADDRSTRLEN);
		hostInfo.port = ::ntohs(address.sin_port);

		return hostInfo;
	}
}