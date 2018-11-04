#include "texGen.h"
#include "float.h"

void genVoronoiMap(int width, uint32_t *tex, float radius, float variation) {

	int numPoints = (int)((width/radius) * (width/radius));

	weightV2 points[numPoints];

	for(int i = 0; i < (int)(width/radius); ++i) {
		for(int j = 0; j < (int)(width/radius); ++j) {
			int index = j + i*(int)(width/radius);
			points[index].x = (frand(variation*radius) -
							   variation*radius/4) + i*radius +
							   radius/2;
			points[index].y = (frand(variation*radius) -
							   variation*radius/4) + j*radius +
							   radius/2;
			points[index].weight = frand(1.f);
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

			// tex[index] = dist < 2 ? 0xffff0000 :
 			// 			 0xff << 24 |
 			// 			 (int)((points[nextClosest].weight) * 0xff) << 8;
			tex[index] = 0xff << 24 |
 						 (int)((points[nextClosest].weight) * 0xff) << 8;

        }
    }

}
