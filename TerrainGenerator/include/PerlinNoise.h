#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <cmath>
#include <vector>

typedef struct {
    float x, y;
} vector2;

vector2 randomGradient(int ix, int iy);
float dotGridGradient(int ix, int iy, float x, float y);
float interpolate(float a0, float a1, float w);
float perlin(float x, float y);
std::vector<std::vector<float>> generatePerlinNoise(int width, int height);

#endif // PERLIN_NOISE_H