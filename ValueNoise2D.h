//
// Created by Liam Murphy on 2022/08/11.
//
#pragma once
#include "Utility.h"
#include "INoiseGen2D.h"

class ValueNoise2D : public INoiseGen2D
{

public:
    float sample(float x, float y);
    ValueNoise2D(unsigned seed = 1234);
    virtual ~ValueNoise2D() {};

private:
    static const unsigned int LUTSIZE = 256;
    static const unsigned int LUTBITMASK = LUTSIZE - 1;
    unsigned int indexLUT[LUTSIZE * 2];
    float randomFloatTable[LUTSIZE];
};