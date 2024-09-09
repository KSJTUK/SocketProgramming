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
