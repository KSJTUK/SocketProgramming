#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "Protocol.h"
#include "Macros.h"
#include "Random.h"

#include <fstream>

#include "../TCPClientWithGUI/pch.h"

inline constexpr size_t ENTITIES = 2000;

void GenerateRandomEntity(size_t numOfEntity)
{
	std::ofstream out{ "Entities.bin", std::ios::binary };
	if (not out) {
		return;
	}

	std::vector<Position> positions(numOfEntity);
	for (auto& position : positions) {
		position = { Random::GetUniformRandFloat(100.0f, static_cast<float>(WORLD_SIZE.cx - 100)),
					Random::GetUniformRandFloat(100.0f, static_cast<float>(WORLD_SIZE.cy - 100)), };
	}

	out.write(reinterpret_cast<char*>(&numOfEntity), sizeof(size_t));
	out.write(reinterpret_cast<char*>(positions.data()), positions.size() * sizeof(Position));
}

int main()
{

}