//
// Created by Liam Murphy on 2022/08/11.
//
#pragma once

class INoiseGen2D
{
public:
    virtual float sample(float x, float y) = 0;
};