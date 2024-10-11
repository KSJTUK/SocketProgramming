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
    void Update(const float deltaTime)
    {
        for (int key = 0; key < MAX_KEY_BUFFER; ++key) {
            Execute(key, deltaTime);
        }
    }

    void KeyUpdate(int key, bool opt)
    {
        mKeyDown[key] = opt;
    }

    void Execute(int key, const float deltaTime)
    {
        if (mKeyDown[key]) {
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
    bool mKeyDown[MAX_KEY_BUFFER]{ false };
    std::array<std::function<void(float)>, MAX_KEY_BUFFER> mExecutionFn{ NULLFUNCTION };
};