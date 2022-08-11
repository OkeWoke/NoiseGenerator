//
// Created by Liam Murphy on 2022/08/11.
//

#include "PerlinNoise2D.h"

PerlinNoise2D::PerlinNoise2D(unsigned int seed)
{
    for (unsigned int i = 0; i< LUTSIZE; i++)
    {
        randomVecTable[i] = Vec2::polarToVec2(Utility::SampleRandom<float>(seed, 0, 2 * PI)); // values between 0 and 1.
        indexLUT[i] = i;
    }

    std::uniform_int_distribution<unsigned> distributionUInt;

    for(unsigned int i = 0; i < LUTSIZE; i++)
    {
        unsigned int randInt = Utility::SampleRandom<int>(seed, 0, LUTSIZE-1);
        std::swap(indexLUT[randInt], indexLUT[i]); // we randomise the LUT.
        indexLUT[i+LUTSIZE] = indexLUT[i];
    }
}
float PerlinNoise2D::sample(float x, float y)
{
    int y0 = std::floor(y);
    int x0 = std::floor(x);

    float tx = x - x0;
    float ty = y - y0;

    y0 = y0 & LUTBITMASK; //wrap around this gives noise function periodicity
    x0 = x0 & LUTBITMASK;

    int y1 = (y0 + 1) & LUTBITMASK;
    int x1 = (x0 + 1) & LUTBITMASK;

    Vec2 ve00 = randomVecTable[indexLUT[indexLUT[x0]+y0]];
    Vec2 ve01 = randomVecTable[indexLUT[indexLUT[x0]+y1]];
    Vec2 ve10 = randomVecTable[indexLUT[indexLUT[x1]+y0]];
    Vec2 ve11 = randomVecTable[indexLUT[indexLUT[x1]+y1]];

    Vec2 pe00, pe01, pe10, pe11;
    pe00 = Vec2{tx, ty};
    pe01 = Vec2{tx, ty-1};
    pe10 = Vec2{tx-1, ty};
    pe11 = Vec2{tx-1, ty-1};

    pe00.normalise();
    pe01.normalise();
    pe10.normalise();
    pe11.normalise();

    float v00 = (ve00.dot(pe00) + 1)/2;
    float v01 = (ve01.dot(pe01) + 1)/2;
    float v10 = (ve10.dot(pe10) + 1)/2;
    float v11 = (ve11.dot(pe11) + 1)/2;

    tx = Utility::smoothstep<float>(tx);
    ty = Utility::smoothstep<float>(ty);
    float lx0 = Utility::lerp(v00, v10, tx);
    float lx1 = Utility::lerp(v01, v11, tx);

    float res = Utility::lerp(lx0, lx1, ty);
    return res;

}