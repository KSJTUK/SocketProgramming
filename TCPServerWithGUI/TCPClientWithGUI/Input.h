#pragma once

#include <Windows.h>
#define MAX_BUFFER 256


class KeyInput {
public:
    KeyInput() = default;
    ~KeyInput() = default;

public:
    void Input() 
    {
        GetKeyboardState(mKeyInfo);
    }

    void Reset() 
    {
        memset(mKeyInfo, false, MAX_BUFFER);
    }

    byte& operator[](int idx) 
    {
        return mKeyInfo[idx];
    }

    void KeyDown(int keyIndex)
    {

    }

    void KeyUp(int keyIndex)
    {

    }

private:
    byte mKeyInfo[MAX_BUFFER];
};