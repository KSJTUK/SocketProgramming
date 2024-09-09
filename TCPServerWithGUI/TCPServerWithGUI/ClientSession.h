#pragma once

/* ----------------------------------------
*
*				ClientSession
* 
*	클라이언트에 대한 기본정보들
*	(ex: position, id, name 등등...)을
*	저장하고 클라이언트 최초 접속시 
*	세션 정보를 전송할 클래스
* 
  ---------------------------------------- */

class ClientSession {
	inline static constexpr byte MAX_NAME_BYTE = 32;

public:
	ClientSession();
	ClientSession(byte clientId);
	~ClientSession();

public:
	byte GetId() const { return mId; }
	std::pair<float, float> GetPosition() const { return std::make_pair(x, y); }

private:
	byte mId{ 0xFF };
	char mName[MAX_NAME_BYTE + 1]{ };
	float x{ };
	float y{ };
};