#pragma once

#include <Windows.h>
#define MAX_BUFFER 256

struct KEYINFO {
    bool down{ false };
    bool pressed{ false };
    bool up{ false };
};

class KeyInput {
public:
    KeyInput() = default;
    ~KeyInput() = default;

public:
    void Input() {
        for (int keyIndex = 0; keyIndex < MAX_BUFFER; ++keyIndex) {
            Input(keyIndex);
        }
    }

    void Reset() {
        memset(keyInfo, false, MAX_BUFFER);
    }

    KEYINFO& operator[](int idx) {
        return keyInfo[idx];
    }

private:
    void Input(int keyIndex)
    {
        if (GetAsyncKeyState(keyIndex) & 0x8000) {
            if (keyInfo[keyIndex].down) {
                keyInfo[keyIndex].pressed = true;
                keyInfo[keyIndex].down = false;
            }
            else {
                if (not keyInfo[keyIndex].pressed) {
                    keyInfo[keyIndex].down = true;
                }
            }
        }
        else {
            if (keyInfo[keyIndex].up) {
                keyInfo[keyIndex].up = false;
            }
            else if (keyInfo[keyIndex].down || keyInfo[keyIndex].pressed) {
                keyInfo[keyIndex].up = true;
                keyInfo[keyIndex].pressed = false;
                keyInfo[keyIndex].down = false;
            }
        }
    }

private:
    KEYINFO keyInfo[MAX_BUFFER]{ };
};