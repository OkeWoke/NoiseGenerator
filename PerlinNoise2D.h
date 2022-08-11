//
// Created by Liam Murphy on 2022/08/11.
//
#pragma once
#include "Utility.h"
#include "INoiseGen2D.h"

class PerlinNoise2D : INoiseGen2D
{
    float sample(unsigned int x, unsigned int y)
    {
        return 0.0f;
    }
};