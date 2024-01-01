//
// Created by Liam Murphy on 2022/08/11.
//
#pragma once
#include "Utility.h"
#include "INoiseGen2D.h"
#include "Vec2.h"


class PerlinNoise2D : public INoiseGen2D
{
public:
    PerlinNoise2D(unsigned seed);
    float sample(float x, float y);
private:
    const float PI = 3.14159f;
    static const unsigned int LUTSIZE = 256;
    static const unsigned int LUTBITMASK = LUTSIZE - 1;
    unsigned int indexLUT[LUTSIZE * 2];
    Vec2 randomVecTable[LUTSIZE];
};
