#include "texGen.h"
#include "float.h"

void genWeightedBlueNoise(weightV2 *array, float radius, int size, int k) {
    // Poisson Disc Distribution - Blue Noise
    // k is the number of tries in generating points before giving up

	// initiate an array representing a grid of size/radius by size/radius blocks,
	// so we can map points at maximum at one per block

	float w = radius / sqrt(2);
	int cols = (int)(size/w);
	int rows = (int)(size/w);

	int gridSize = (int)(cols * rows);
	weightV2 grid[gridSize];
	for(int i = 0; i < gridSize; ++i) grid[i] = (weightV2){-1,-1, 0.f};

	// malloc input array
	int arraySize = 0;
	array = malloc(sizeof(weightV2) * arraySize);

	int x = rand() % size;
	int y = rand() % size;
	int i = (int)(x / size);
	int j = (int)(y / size);
	weightV2 pos = {x, y, frand(1.f)};
	grid[i + j * cols] = pos;
	++arraySize;
	array = realloc(array, sizeof(weightV2) * arraySize);
	printf("cheguei aqui\n");
	printf("arraysize: %d\n", arraySize);

	while(arraySize > 0) {
		printf("teste\n");
		int index = rand() % arraySize;
		printf("to no indice %d", index);
		bool found = false;
		for(int i = 0; i < k; ++i) {
			float theta = frand(2*M_PI);
			float offsetX = cosf(theta) * (frand(radius) + radius);
			float offsetY = sinf(theta) * (frand(radius) + radius);
			weightV2 sample = {offsetX + array[index].x,
							   offsetY + array[index].y,
							   frand(1.f)};
		    int colPos = (int)(sample.x / w);
			int rowPos = (int)(sample.y / w);

			bool valid = true;
			if(grid[colPos + rowPos * rows].x == -1 &&
			   grid[colPos + rowPos * rows].y == -1 &&
		       grid[colPos + rowPos * rows].weight == 0.f) {

				for(int j = -1; j < 2; ++j) {
					for(int k = -1; k < 2; ++k) {
						int newIndex = (colPos+j) + (rowPos+k) * cols;
						weightV2 neighbor = grid[newIndex];
						if(neighbor.x != -1 &&
						   neighbor.y != -1 &&
						   neighbor.weight != 0.f) {
							float dist = points_distance((v3){sample.x, sample.y,0},
														 (v3) {neighbor.x, neighbor.y,0});
							if(dist < radius) {
								valid = false;
							}
						}
					}
				}

				if(valid) {
					found = true;
					grid[colPos + rowPos*cols] = sample;
					array[arraySize] = sample;
					arraySize++;
					array = realloc(array, sizeof(weightV2)*arraySize);
					break;
				}

			}

			if(!found) {
				arraySize--;
				array = realloc(array, sizeof(weightV2) * arraySize);
				printf("Diminui a lista\n");
			}

		}
	}

}

void genVoronoiMap(int width, uint32_t *tex, float radius) {

	float w = radius / sqrt(2);
	int cols = (int)(width/w);
	int rows = (int)(width/w);

	int numPoints = (int)(cols * rows);

	weightV2 *points = malloc(sizeof(weightV2));
	genWeightedBlueNoise(points, radius, width, 30);
	// printf("Num Points: %d\n", numPoints);

	for(int i = 0; i < width; ++i) {
        for(int j = 0; j < width; ++j) {
            int index = j + i*width;

			// printf("Chegou aqui! indice %d\n", index);

            uint32_t dist = UINT_MAX;
            int nextClosest = 0;
            for(int k = 0; k < numPoints; k++) {
				// printf("k index: %d\n", k);
                int dx = abs(j-points[k].x);
                int dy = abs(i-points[k].y);
                int newDist = intSqDist(dx, dy);

                if(dist > newDist) {
                    dist = newDist;
                    nextClosest = k;
                }

            }

			// printf("passou daqui! Indice %d\n", index);

            tex[index] = dist < 2 ? 0xff0000ff : 0xffffffff;
						 // points[nextClosest].weight * 0xffffffff || 0xff << 24;
            // tex[index] = points[nextClosest].;

        }
    }

    // Calculate time taken
    // end = clock();
    // timeTaken = (double)(end - start)/1000.f;
    // printf("%lf ms taken\n", timeTaken);

	free(points);

}

void genVoronoiTex(int width, int numPoints, uint32_t *tex, distenum distType) {

    // start = clock();
    //v2i points[numPoints];
    //uint32_t colors[numPoints];

	v2i *points = malloc(sizeof(v2i) * numPoints);
	uint32_t *colors = malloc(sizeof(uint32_t) * numPoints);

    // initialize points coordinates
    // randomGenV2i(points, numPoints, 2, width-2, WHITE_NOISE);
    for(int i = 0; i < numPoints;i++) {
        colors[i] = 0xff << 24 |
                    (30 + rand() % 136) << 16 |
                    (30 + rand() % 136) << 8 |
                    (30 + rand() % 136);
    }

    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < width; ++j) {
            int index = j + i*width;

            uint32_t dist = UINT_MAX;
            int nextClosest = 0;
            for(int k = 0; k < numPoints; k++) {
                // calculate distances considering wrap around, for seamless textures
                int dx = abs(j-points[k].x) > width/2 ? width-abs(j-points[k].x) : abs(j-points[k].x);
                int dy = abs(i-points[k].y) > width/2 ? width-abs(i-points[k].y) : abs(i-points[k].y);
                int newDist = distType == EUCLIDEAN ? intSqDist(dx, dy) :
                                                      intManDist(dx, dy);

                if(dist > newDist) {
                    dist = newDist;
                    nextClosest = k;
                }

            }

            // tex[index] = dist < 2 ? 0xffff0000 : colors[nextClosest];
            tex[index] = colors[nextClosest];

        }
    }

    // Calculate time taken
    // end = clock();
    // timeTaken = (double)(end - start)/1000.f;
    // printf("%lf ms taken\n", timeTaken);

	free(points);
	free(colors);
}
