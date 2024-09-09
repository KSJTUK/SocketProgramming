#include "pch.h"
#include "ClientSession.h"

/* ----------------------------------------
*
*				ClientSession
*
  ---------------------------------------- */

ClientSession::ClientSession() = default;

ClientSession::ClientSession(byte clientId)
	: mId{ clientId }
{
}

ClientSession::~ClientSession() = default;

void ClientSession::SetPosition(float x, float y)
{
	mX = x;
	mY = y;
}
