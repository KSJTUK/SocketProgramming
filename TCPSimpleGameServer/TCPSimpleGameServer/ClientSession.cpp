#include "pch.h"
#include "ClientSession.h"

/* ----------------------------------------
*
*				ClientSession
*
  ---------------------------------------- */

ClientSession::ClientSession() = default;

ClientSession::~ClientSession() = default;

void ClientSession::SetPosition(float x, float y)
{
	mX = x;
	mY = y;
}

void ClientSession::SetId(byte id)
{
	mId = id;
}
