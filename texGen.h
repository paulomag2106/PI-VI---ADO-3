#pragma once
#include "functions.h"

clock_t start, end;
double timeTaken;

typedef enum t_distenum {

    EUCLIDEAN,
    MANHATTAN

} distenum;

typedef struct t_weightV2 {
    uint32_t x,y;
    float weight;
} weightV2;

void genWeightedBlueNoise(weightV2 *array, v2i radius, v2i size);
void genVoronoiTex(int width, int numPoints, uint32_t *tex, distenum distType);
