//
// Created by Liam Murphy on 2022/08/11.
//
#pragma once
#include <random>
#include <type_traits>

// We template the uniform distribution such that float and int based code is interchangeable.
template<typename T>
using uniform_distribution =
typename std::conditional<
        std::is_floating_point<T>::value,
        std::uniform_real_distribution<T>,
        typename std::conditional<
                std::is_integral<T>::value,
                std::uniform_int_distribution<T>,
                void
        >::type
>::type;

class Utility
{
public:
    template <class T>
    static T SampleRandom(unsigned int seed, T min = 0, T max = 1)
    {
        static thread_local std::mt19937 gen(seed); // not recreated each call
        uniform_distribution<T> distribution(min, max);
        return distribution(gen);
    }

    template <typename T = float>
    static inline T lerp(const T&  min, const T& max, const T& t)
    {
        return min * (1 - t) + max * t;
    }

    template <typename T = float>
    static inline T smoothstep(const T& t)
    {
        return t * t * ( 3 - 2 * t);
    }
};
