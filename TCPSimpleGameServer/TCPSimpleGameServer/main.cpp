#include "pch.h"

int main(int argc, char* argv[])
{
	gGameServer.Init();

	while (true) {
		gGameServer.Update();
	}

	gGameServer.Join();
}