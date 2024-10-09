#pragma once

/*----------------------------------
		���� ���� ������ �Լ���
----------------------------------*/

// ���� �޽��� ��ȯ
inline void* GetErrorMessage()
{
    int errorCode = ::WSAGetLastError();

    void* errorMessage = nullptr;
    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<char*>(&errorMessage),
        0,
        nullptr
    );
    
    return errorMessage;
}

// ���� �Ҵ��� ���� �޽��� ��ȯ
inline std::string GetErrorString()
{
    int errorCode = ::WSAGetLastError();

    void* errorMessage = nullptr;
    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<char*>(&errorMessage),
        0,
        nullptr
    );

    std::string errorString{ reinterpret_cast<char*>(errorMessage) };

    LocalFree(errorMessage);

    return errorString;
}

/*----------------------------------
				Crash
			������ ����
----------------------------------*/

#define Crash(cause)												\
{																	\
    char* errorMessage = reinterpret_cast<char*>(GetErrorMessage());\
	MessageBoxA(nullptr, errorMessage, cause, MB_OK);	            \
	int* crash = nullptr;											\
	__analysis_assume(crash != nullptr);							\
	*crash = 0xFFFFFFFF;											\
}

#define ASSERT(expr, cause)									        \
{																	\
	if (not (expr)) {												\
		Crash(cause);												\
	}																\
}																

