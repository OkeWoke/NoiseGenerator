//
// Created by Liam Murphy on 2022/08/12.
//
#pragma once
#include "math.h"

struct Vec2
{
    float x;
    float y;

    static Vec2 polarToVec2(float polar)
    {
        Vec2 vec;
        vec.x = cos(polar);
        vec.y = sin(polar);
        return vec;
    }

    float dot(Vec2& rhs)
    {
        return this->x * rhs.x + this->y * rhs.y;
    }

    void normalise()
    {
        float length = sqrt(this->x*this->x + this->y * this->y);
        this->x = this->x / length;
        this->y = this->y / length;
    }
};