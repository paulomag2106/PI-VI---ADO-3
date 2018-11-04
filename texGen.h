#pragma once
#include "functions.h"

clock_t start, end;
double timeTaken;

typedef enum t_distenum {

    EUCLIDEAN,
    MANHATTAN

} distenum;

typedef struct t_weightV2 {
    int x,y;
    float weight;
} weightV2;

void genVoronoiMap(int width, uint32_t *tex, float radius, float variation); // variation 0 to 1
void genVoronoiTex(int width, int numPoints, uint32_t *tex, distenum distType);
