#pragma once

#include <Windows.h>
#include <functional>

inline constexpr unsigned short MAX_KEY_BUFFER = 256;
inline constexpr void(*NULLFUNCTION)(float) = [](float=0){};

class KeyInput {
    inline static constexpr byte ACTIVE_HI_BIT = 0xF0;
    inline static constexpr byte ACTIVE_LO_BIT = 0x01;

public:
    KeyInput()
    {
        for (short i = 0; i < MAX_KEY_BUFFER; ++i) {
            RegisterKeyFn(i);
        }
    }

    ~KeyInput() = default;

public:
    void Input(float deltaTime)
    {
        bool result = GetKeyboardState(mKeyInfo);
        for (unsigned short key = 0; key < MAX_KEY_BUFFER; ++key) {
            Execute(key, deltaTime);
        }
    }

    void Reset() 
    {
        memset(mKeyInfo, false, MAX_KEY_BUFFER);
    }

    byte& operator[](int idx) 
    {
        return mKeyInfo[idx];
    }

    void Execute(int key, float deltaTime)
    {
        if (mKeyInfo[key] & 0xF0) {
            mExecutionFn[key](deltaTime);
        }
    }

    void RegisterKeyFn(unsigned short key, std::function<void(float)>&& fn = NULLFUNCTION)
    {
        mExecutionFn[key] = fn;
    }

private:
    byte mKeyInfo[MAX_KEY_BUFFER]{ };
    std::unordered_map<unsigned short, std::function<void(float) >> mExecutionFn{ };
};