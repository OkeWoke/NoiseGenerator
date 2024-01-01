//
// Created by Liam Murphy on 2022/08/11.
//

#include "ValueNoise2D.h"
#include "Utility.h"
ValueNoise2D::ValueNoise2D(unsigned seed)
{
    for (unsigned int i = 0; i< LUTSIZE; i++)
    {
        randomFloatTable[i] = Utility::SampleRandom<float>(seed, 0, 1); // values between 0 and 1.
        indexLUT[i] = i;
    }

    std::uniform_int_distribution<unsigned> distributionUInt;

    for(unsigned int i = 0; i < LUTSIZE; i++)
    {
        unsigned int randInt = Utility::SampleRandom<int>(seed, 0, LUTSIZE-1);
        std::swap(indexLUT[randInt], indexLUT[i]); // we randomise the LUT.
        indexLUT[i+LUTSIZE] = indexLUT[i];
    }
    // create array of random floats and permutation table;
}

float ValueNoise2D::sample(float x, float y) // R^2 -> R
//returns values between 0 and 1
{
    int y0 = std::floor(y);
    int x0 = std::floor(x);

    float tx = x - x0;
    float ty = y - y0;

    y0 = y0 & LUTBITMASK; //wrap around this gives noise function periodicity
    x0 = x0 & LUTBITMASK;

    int y1 = (y0 + 1) & LUTBITMASK;
    int x1 = (x0 + 1) & LUTBITMASK;

    float v00 = randomFloatTable[indexLUT[indexLUT[x0]+y0]];
    float v01 = randomFloatTable[indexLUT[indexLUT[x0]+y1]];
    float v10 = randomFloatTable[indexLUT[indexLUT[x1]+y0]];
    float v11 = randomFloatTable[indexLUT[indexLUT[x1]+y1]];

    tx = Utility::smoothstep<float>(tx);
    ty = Utility::smoothstep<float>(ty);
    float lx0 = Utility::lerp(v00, v10, tx);
    float lx1 = Utility::lerp(v01, v11, tx);

    float res = Utility::lerp(lx0, lx1, ty);
    return res;
}