#include "pch.h"
#include "GameServer.h"
#include "Listener.h"
#include "Timer.h"
#include "Object.h"

/* ----------------------------------------
*
*				GameServer
*
  ---------------------------------------- */

GameServer::GameServer() = default;
GameServer::~GameServer() = default;

void GameServer::ProcessPacket(char* packet)
{
	// PacketBase 구조체에서 1바이트는 size, 1바이트는 type, 1바이트는 송신자 id로 설정했음
	byte size = packet[0];
	byte type = packet[1];
	byte senderId = packet[2];

	switch (type) {
	case PACKET_PLAYER_CONNECT:
		std::cout << "Player Connected" << std::endl;
		Broadcast<PacketPlayerJoin>(PACKET_PLAYER_JOIN, senderId, packet);
		SendOtherClientsSession(senderId);
		break;

	case PACKET_PLAYER_INPUT:
	{
		PacketPlayerInput* input = reinterpret_cast<PacketPlayerInput*>(packet);
		mClients[senderId]->GetInputProcessor().KeyUpdate(input->keyInput, input->down);
	}
	break;

	case PACKET_PLAYER_JOIN:
		break;

	case PACKET_PLAYER_EXIT:
		std::cout << "Player Exited" << std::endl;
		Broadcast<PacketPlayerExit>(PACKET_PLAYER_EXIT, senderId, packet);
		ExitClient(senderId);
		break;

	case PACKET_PING:
		mClients[senderId]->GetTransceiver().Send<PacketPing>(PACKET_PING, senderId, packet);
		break;
	}
}

void GameServer::SendClientsInfo()
{
	PacketPosition2D packet{ sizeof(PacketPosition2D), PACKET_POSITION2D, };

	for (int clientId = 0; clientId < MAX_CLIENT; ++clientId) {
		std::lock_guard stateGuard{ mClients[clientId]->GetMutex() };
		if (mClients[clientId]->GetState() == CLIENT_STATE::JOINED) {
			packet.pos = mClients[clientId]->GetPosition();
			Broadcast<PacketPosition2D>(PACKET_POSITION2D, clientId, reinterpret_cast<char*>(&packet));
		}
	}
}

void GameServer::SendObjectsInfo()
{
	PacketObjectInfo packet{ sizeof(PacketObjectInfo), PACKET_OBJECT_INFO, };

	for (unsigned short count{ 0 }; auto& object : mObjects) {
		if (not object)
			continue;

		packet.pos = object->GetPosition();
		packet.boxSize = object->GetBoxSize();
		packet.dir = object->GetDirection();
		packet.velocity = object->GetVelocity();
		packet.color = object->GetColor();
		packet.objectType = object->GetType();
		packet.objectIndex = count++;

		for (int clientId = 0; clientId < MAX_CLIENT; ++clientId) {
			std::lock_guard stateGuard{ mClients[clientId]->GetMutex() };
			if (mClients[clientId]->GetState() == CLIENT_STATE::JOINED) {
				mClients[clientId]->GetTransceiver().Send(&packet);
			}
		}
	}
}

Object* GameServer::CreateObject(OBJECT_TYPE objType, Position pos, SIZE size, DWORD color)
{
	// TODO 맘에 안듬
	switch (objType) {
	case WALL:
		return new Wall{ pos, size, color };

	case BULLET:
		return new Bullet{ pos, size, color };
	}

	return nullptr;
}

bool GameServer::AllocObject(OBJECT_TYPE objType, Position pos, SIZE size, DWORD color)
{
	// TODO 너무 대충만들었다.
	// 오브젝트 부분은 Object Pool을 이용하는게 좋을거 같음. 변경 예정.
	for (int allocIdx{ }; auto& alive : mObjectAlive) {
		if (not alive) {
			alive = true;
			mObjects[allocIdx].reset(CreateObject(objType, pos, size, color));
			return true;
		}

		++allocIdx;
	}

	return false;
}

void GameServer::Update()
{
	static float updateDelay = 0.f;
	static const float delayed = 1.0f / 60.0f;
	static bool sendSwitch = false;

	mTimer->Update();
	mDeltaTime = mTimer->GetDeltaTime();

	updateDelay += mDeltaTime;
	if (updateDelay < delayed) {
		return;
	}

	mDeltaTime = updateDelay;

	for (auto& client : mClients) {
		client->Update(mDeltaTime.load());
	}

	for (auto& object : mObjects) {
	    if (object)
	        object->Update(mDeltaTime.load());
	}

	if (sendSwitch) {
		SendClientsInfo();
	}
	else {
		SendObjectsInfo();
	}

	sendSwitch = !sendSwitch;
	updateDelay = 0.0f;
}

void GameServer::CreateCoreObjects()
{
	mListener = std::make_unique<Listener>(SERVER_IP, SERVER_PORT);
	mTimer = std::make_unique<Timer>();
	for (int id = 0; id < MAX_CLIENT; ++id) {
		mClients.emplace_back(std::make_shared<Client>());
	}
}

void GameServer::Accept()
{
	while (true) {
		SOCKET clientSocket = mListener->Accept();
		if (INVALID_SOCKET == clientSocket) {
			continue;
		}

		byte addedId = AddClient(clientSocket);
		if (NULL_CLIENT_ID == addedId) {
			continue;
		}

		// 람다를 이용해 생성
		// 처음 연결시 클라이언트에게 ID를 부여하는 패킷 전송
		mClientServiceThreads.push_back(std::thread{ [=]()
			{
				byte id = addedId;
				mClients[id]->GetTransceiver().Send<PacketPlayerConnect>(PACKET_PLAYER_CONNECT, id);

				mClients[id]->GetTransceiver().Recv();
			}
			});
	}
}

void GameServer::Init()
{
	WSAData wsaData;
	ASSERT(0 == ::WSAStartup(MAKEWORD(2, 2), &wsaData), "WSAStartup Failure");

	CreateCoreObjects();
	mAcceptThread = std::thread{ [=]() { Accept(); } };

	mObjects.resize(MAX_OBJECT);
	for (int i = 0; i < 500; ++i) {
		AllocObject(
			WALL,
			Position{ Random::GetUniformRandom<float>(0.0f, WORLD_SIZE.cx), Random::GetUniformRandom<float>(0.0f, WORLD_SIZE.cy) },
			SIZE{ Random::GetUniformRandom<long>(40, 100), Random::GetUniformRandom<long>(40, 100) },
			Random::GetRandomRGB()
		);
	}
}

void GameServer::Join()
{
	for (std::thread& thread : mClientServiceThreads) {
		if (thread.joinable()) {
			thread.join();
		}
	}

	mAcceptThread.join();
	::WSACleanup();
}

std::vector<std::shared_ptr<Client>>& GameServer::GetClients()
{
	return mClients;
}

byte GameServer::AddClient(SOCKET clientSocket)
{
	for (byte id = 0; id < MAX_CLIENT; ++id) {
		{
			// 클라이언트의 상태를 변경하는 부분은 locking
			std::lock_guard stateGuard{ mClients[id]->GetMutex() };
			if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
				continue;
			}
			mClients[id]->Join(clientSocket, id);
		}

		Address::NetHostInfo hostInfo = Address::GetHostInfo(clientSocket);


		std::cout << "[클라이언트 접속" << " ID: " << (int)id << "] IP: " << hostInfo.ip << " | PORT: " << hostInfo.port << "\n";
		return id;
	}

	return NULL_CLIENT_ID;
}

void GameServer::ExitClient(byte id)
{
	std::lock_guard stateGuard{ mClients[id]->GetMutex() };
	if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
		auto& [ip, port] = mClients[id]->GetTransceiver().GetHostInfo();
		std::cout << "[클라이언트 연결 종료] IP: " << ip << " | PORT: " << port << "\n";
		mClients[id]->GetTransceiver().CloseSocket();
		mClients[id]->Exit();
	}
}

void GameServer::SendOtherClientsSession(byte targetId)
{
	PacketPlayerJoin packet{ sizeof(PacketPlayerJoin), PACKET_PLAYER_JOIN };
	for (int id = 0; id < MAX_CLIENT; ++id) {
		std::lock_guard stateGuard{ mClients[id]->GetMutex() };
		if (mClients[id]->GetState() == CLIENT_STATE::JOINED) {
			packet.senderId = id;
			packet.pos = mClients[id]->GetPosition();
			mClients[targetId]->GetTransceiver().Send(&packet);
		}
	}
}