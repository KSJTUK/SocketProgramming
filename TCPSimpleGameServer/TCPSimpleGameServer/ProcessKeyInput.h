#pragma once

inline constexpr unsigned short MAX_KEY_BUFFER = 256;
inline constexpr void(*NULLFUNCTION)(float) = [](float = 0) {};

class ProcessKeyInput {
public:
    ProcessKeyInput()
    {
        for (short i = 0; i < MAX_KEY_BUFFER; ++i) {
            RegisterKeyFn(i, NULLFUNCTION);
        }
    }

    ~ProcessKeyInput() = default;

public:
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

    [[maybe_unused]] std::function<void(float)> RegisterKeyFn(unsigned short key, std::function<void(float)>&& fn)
    {
        auto prevFunction = mExecutionFn[key];
        mExecutionFn[key] = fn;
        return prevFunction;
    }

private:
    byte mKeyInfo[MAX_KEY_BUFFER]{ };
    std::array<std::function<void(float)>, 256> mExecutionFn{ };
};