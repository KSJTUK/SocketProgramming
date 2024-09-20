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
#include <unordered_map>

// Commons
#include "Macros.h"
#include "Random.h"
#include "Protocol.h"
#include "Endian.h"

#include "TCPServerCore.h"

namespace Address {
	struct NetHostInfo {
		char ip[INET_ADDRSTRLEN]{ };
		unsigned short port;
	};

	inline NetHostInfo GetHostInfo(SOCKET socket)
	{
		NetHostInfo hostInfo{ };

		sockaddr_in address;
		int addressLength{ sizeof(sockaddr_in) };
		::getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressLength);

		::inet_ntop(AF_INET, &address.sin_addr, hostInfo.ip, INET_ADDRSTRLEN);
		hostInfo.port = ::ntohs(address.sin_port);

		return hostInfo;
	}
}

extern TCPServerCore gServerCore;