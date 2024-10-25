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

ObjectPool<class Wall, OBJECT_POOL_MAX::OBJECT_POOL_WALL> GameServer::walls;
ObjectPool<class Ball, OBJECT_POOL_MAX::OBJECT_POOL_BALL> GameServer::balls;

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

void GameServer::UpdateCollision(float deltaTime)
{
	for (int clientId = 0; clientId < MAX_CLIENT; ++clientId) {
		auto& client = mClients[clientId];

		//std::lock_guard stateGuard{ mClients[clientId]->GetMutex() };
		if (client->GetState() != CLIENT_STATE::JOINED) {
			continue;
		}

		for (auto& wall : mWalls) {
			if (wall and client->CheckCollision(wall)) {
				client->HandleCollision(wall);
			}
		}
	}

	//for (auto& ball : mBalls) {
	//	for (auto& wall : mWalls) {
	//		if (ball->CheckCollision(wall)) {
	//			ball->HandleCollision(wall);
	//			wall->HandleCollision(ball);
	//		}
	//	}
	//}
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

	for (unsigned short count{ 0 }; auto& ball : mBalls) {
		if (not ball)
			continue;

		packet.pos = ball->GetPosition();
		packet.boxSize = ball->GetBoxSize();
		packet.dir = ball->GetDirection();
		packet.velocity = ball->GetVelocity();
		packet.color = ball->GetColor();
		packet.objectType = ball->GetType();
		packet.objectIndex = count++;

		for (int clientId = 0; clientId < MAX_CLIENT; ++clientId) {
			std::lock_guard stateGuard{ mClients[clientId]->GetMutex() };
			if (mClients[clientId]->GetState() == CLIENT_STATE::JOINED) {
				mClients[clientId]->GetTransceiver().Send(&packet);
			}
		}
	}
}

void GameServer::SendStaticObjectInfo()
{
	PacketObjectInfo packet{ sizeof(PacketObjectInfo), PACKET_OBJECT_INFO, };

	for (unsigned short count{ 0 }; auto & wall : mWalls) {
		if (not wall)
			continue;

		packet.pos = wall->GetPosition();
		packet.boxSize = wall->GetBoxSize();
		packet.dir = wall->GetDirection();
		packet.velocity = wall->GetVelocity();
		packet.color = wall->GetColor();
		packet.objectType = wall->GetType();
		packet.objectIndex = count++;

		for (int clientId = 0; clientId < MAX_CLIENT; ++clientId) {
			std::lock_guard stateGuard{ mClients[clientId]->GetMutex() };
			if (mClients[clientId]->GetState() == CLIENT_STATE::JOINED) {
				mClients[clientId]->GetTransceiver().Send(&packet);
			}
		}
	}
}

void GameServer::Update()
{
	static float updateDelay = 0.f;
	static const float delayed = 1.0f / 30.0f;
	static bool sendSwitch = false;

	mTimer->Update();
	mDeltaTime = mTimer->GetDeltaTime();

	updateDelay += mDeltaTime;
	if (updateDelay < delayed) {
		return;
	}

	mDeltaTime = updateDelay;

	if (sendSwitch) {
        for (auto& client : mClients) {
            client->Update(mDeltaTime.load());
        }

		UpdateCollision(mDeltaTime);

		SendClientsInfo();
	}
	else {
		SendStaticObjectInfo();
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

	float border = 300.0f;
	for (int i = 0; i < 100; ++i) {
		auto wall = walls.AcquireObject();

		auto pos = Vec2D{ Random::GetUniformRandom<float>(border, static_cast<float>(WORLD_SIZE.cx) - border), Random::GetUniformRandom<float>(border, static_cast<float>(WORLD_SIZE.cy) - border) };
		auto size = SizeF{ 40.0f, 40.0f };
		auto color = RGB(255, 0, 0);

		wall->SetPosition(pos);
		wall->SetSize(size);
		wall->SetColor(color);
		mWalls.push_back(wall);
	}

	for (int i = 0; i < balls.Size(); ++i) {
		auto ball = balls.AcquireObject();

		auto pos = Vec2D{ Random::GetUniformRandom<float>(border, static_cast<float>(WORLD_SIZE.cx) - border), Random::GetUniformRandom<float>(border, static_cast<float>(WORLD_SIZE.cy) - border) };
		auto size = SizeF{ 10.0f, 10.0f };
		auto color = RGB(0, 0, 255);

		ball->SetPosition(pos);
		ball->SetSize(size);
		ball->SetColor(color);
		mBalls.push_back(ball);
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