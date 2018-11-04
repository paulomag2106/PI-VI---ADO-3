#pragma once
#include "functions.h"

typedef struct t_weightV2 {
    int x,y;
    float weight;
} weightV2;

void genVoronoiMap(int width, uint32_t *tex, float radius, float variation); // variation 0 to 1
