#include <iostream>
#include "opencv2/opencv.hpp"
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

template <class T>
T SampleRandom(unsigned int seed, T min = 0, T max = 1)
{
    static thread_local std::mt19937 gen(seed); // not recreated each call
    uniform_distribution<T> distribution(min, max);
    return distribution(gen);
}

template <typename T = float>
inline T lerp(const T&  min, const T& max, const T& t)
{
    return min * (1 - t) + max * t;
}

template <typename T = float>
inline T smoothstep(const T& t)
{
    return t * t * ( 3 - 2 * t);
}

class INoiseGen2D
{
    virtual float sample(float x, float y) = 0;
};

class ValueNoise2D : INoiseGen2D
{

public:
    ValueNoise2D(unsigned seed = 1234)
    {
        for (unsigned int i = 0; i< LUTSIZE; i++)
        {
            randomFloatTable[i] = SampleRandom<float>(seed, 0, 1); // values between 0 and 1.
            indexLUT[i] = i;
        }

        std::uniform_int_distribution<unsigned> distributionUInt;

        for(unsigned int i = 0; i < LUTSIZE; i++)
        {
            unsigned int randInt = SampleRandom<int>(seed, 0, LUTSIZE-1);
            std::swap(indexLUT[randInt], indexLUT[i]); // we randomise the LUT.
            indexLUT[i+LUTSIZE] = indexLUT[i];
        }
        // create array of random floats and permutation table;
    }

    float sample(float x, float y) // R^2 -> R
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

        //tx = smoothstep<float>(tx);
        //ty = smoothstep<float>(ty);
        float lx0 = lerp(v00, v10, tx);
        float lx1 = lerp(v01, v11, tx);

        float res = lerp(lx0, lx1, ty);
        return res;
    }

private:
    static const unsigned int LUTSIZE = 256;
    static const unsigned int LUTBITMASK = LUTSIZE - 1;
    unsigned int indexLUT[LUTSIZE * 2];
    float randomFloatTable[LUTSIZE];
};

class PerlinNoise2D : INoiseGen2D
{
    float sample(unsigned int x, unsigned int y)
    {
        return 0.0f;
    }
};

int main() {
    std::cout << "Noise Generator" << std::endl;
    const unsigned int WIDTH = 5000;
    const unsigned int HEIGHT = 4000;
    const unsigned int SEED = 1234;
    const unsigned NUM_LAYERS = 8;

    cv::Mat image(HEIGHT, WIDTH, CV_8UC1);
    ValueNoise2D noiser(SEED);


    for(unsigned int x = 0; x < WIDTH; x++)
    {
        for(unsigned int y = 0; y < HEIGHT; y++)
        {
            float value = 0;
            float frequency = 0.005f;
            const float max_amplitude = 8.0f;
            float denominator = max_amplitude;
            float amplitude = max_amplitude;
            for(unsigned l = 0; l < NUM_LAYERS; l++)
            {
                value+= amplitude * noiser.sample(x * frequency, y * frequency);
                denominator+=amplitude;
                frequency*=2;
                amplitude/=2;
            }

            image.at<unsigned char>(y, x) = static_cast<int>(255*value/denominator);
        }
    }

    cv::imshow("Noise Generator", image);
    cv::waitKey(0);
    cv::imwrite("ValueNoiseMap.png", image);

    return 0;
}
