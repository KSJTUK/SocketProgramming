#include "pch.h"
#include "TCPServerCore.h"

int main()
{    
	TCPServerCore core;

	core.Init();

	core.Join();
}