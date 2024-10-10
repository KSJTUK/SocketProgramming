#include "pch.h"
#include "Client.h"

/* ----------------------------------------
*
*				Client
*
  ---------------------------------------- */

inline constexpr float MIN_WORLD_BORDER = 100.0f;

Client::Client()
	: mId{ NULL_CLIENT_ID },
	mPosition{ Random::GetUniformRandFloat(MIN_WORLD_BORDER, WORLD_SIZE.cx - MIN_WORLD_BORDER), Random::GetUniformRandFloat(MIN_WORLD_BORDER, WORLD_SIZE.cy - MIN_WORLD_BORDER) }
{
}

Client::~Client()
{
}

Transceiver& Client::GetTransceiver()
{
	return mTransceiver;
}

const byte Client::GetId() const
{
	return mId;
}

Position Client::GetPosition() const
{
	return mPosition;
}

void Client::SetPosition(const Position pos)
{
	mPosition = pos;
}

void Client::Join(SOCKET clientSocket, byte id)
{
	mTransceiver.Init(clientSocket);
	mId = id;
	mClientState = CLIENT_STATE::JOINED;
}

void Client::Exit()
{
	mId = NULL_CLIENT_ID;
	mClientState = CLIENT_STATE::EXITED;
}