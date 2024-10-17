#pragma once

#include <random>

namespace Random {
    inline std::random_device rd{ };
    inline std::default_random_engine dre{ rd() };

    template <typename T> requires std::is_integral_v<T>
    inline T GetUniformRandIntegral(T min, T max)
    {
        std::uniform_int_distribution<T> uid{ min, max };
        return uid(dre);
    }

    template <typename T> requires std::is_floating_point_v<T>
    inline T GetUniformRandFloat(T min, T max)
    {
        std::uniform_real_distribution<T> urd{ min, max };
        return urd(dre);
    }

    template <typename T> requires std::is_integral_v<T> or std::is_floating_point_v<T>
    inline T GetUniformRandom(T min, T max)
    {
        if constexpr (std::is_integral_v<T>) {
            return GetUniformRandIntegral<T>(min, max);
        }
        else {
            return GetUniformRandFloat<T>(min, max);
        }
    }

    inline DWORD GetRandomRGB()
    {
        DWORD rgb{ };
        rgb |= GetUniformRandIntegral<DWORD>(0, 0xFF) << 16;
        rgb |= GetUniformRandIntegral<DWORD>(0, 0xFF) << 8;
        rgb |= GetUniformRandIntegral<DWORD>(0, 0xFF);
        return rgb;
    }

    inline DWORD GetRandomRGBA()
    {
        DWORD rgba{ };
        rgba |= GetUniformRandIntegral<DWORD>(0, 0xFF) << 24;
        rgba |= GetUniformRandIntegral<DWORD>(0, 0xFF) << 16;
        rgba |= GetUniformRandIntegral<DWORD>(0, 0xFF) << 8;
        rgba |= GetUniformRandIntegral<DWORD>(0, 0xFF);
        return rgba;
    }
}