#pragma once

/* ----------------------------------------
*
*				ClientSession
* 
*	Ŭ���̾�Ʈ�� ���� �⺻������
*	(ex: position, id, name ���...)��
*	�����ϰ� Ŭ���̾�Ʈ ���� ���ӽ� 
*	���� ������ ������ Ŭ����
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