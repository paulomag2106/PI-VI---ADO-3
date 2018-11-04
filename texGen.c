#include "texGen.h"
#include "float.h"

void genVoronoiMap(int width, uint32_t *tex, float radius, float variation) {

	int numPoints = (int)((width/radius) * (width/radius));
	printf("NUM POINTS: %d\n", numPoints);

	weightV2 points[numPoints];
	// genWeightedBlueNoise(points, radius, width);

	for(int i = 0; i < (int)(width/radius); ++i) {
		for(int j = 0; j < (int)(width/radius); ++j) {
			int index = j + i*(int)(width/radius);
			printf("Index: %d\n", index);
			points[index].x = (frand(variation*radius) -
							   variation*radius/4) + i*radius +
							   radius/2;
			points[index].y = (frand(variation*radius) -
							   variation*radius/4) + j*radius +
							   radius/2;
			points[index].weight = frand(1.f);
			printf("point[%d]:(%d,%d)\n", index, points[index].x, points[index].y);
		}
	}

	for(int i = 0; i < width; ++i) {
        for(int j = 0; j < width; ++j) {
            int index = j + i*width;

            uint32_t dist = UINT_MAX;
            int nextClosest = 0;
            for(int k = 0; k < numPoints; k++) {
                int dx = abs(j-points[k].x);
                int dy = abs(i-points[k].y);
                int newDist = intSqDist(dx, dy);

                if(dist > newDist) {
                    dist = newDist;
                    nextClosest = k;
                }

            }

			// printf("passou daqui! Indice %d\n", index);

            tex[index] = dist < 2 ? 0xffff0000 :
						 0xff << 24 |
						 (int)(points[nextClosest].weight * 0xff) << 16 |
						 (int)(points[nextClosest].weight * 0xff) << 8 |
						 (int)(points[nextClosest].weight * 0xff);
            // tex[index] = points[nextClosest].;

        }
    }

    // Calculate time taken
    // end = clock();
    // timeTaken = (double)(end - start)/1000.f;
    // printf("%lf ms taken\n", timeTaken);

	// free(points);

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
