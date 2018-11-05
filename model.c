#include "model.h"

Site sites[NUMPOINTS];


void setNearest(int site_index) {
    
    Site site = sites[site_index];
    
    int value = sqrt(NUMPOINTS);
    int near_index = 0;
    
    
    for(int x = -1; x <= 1; x++) {
        
        for(int y = -1; y <= 1; y++) {
            
            int index = x + (y * value);
            
            if(index != site_index) site.nearest[near_index++] = index;
            
        }
    }
    
}

void createInitialEnvironment() {
    
    for(int i = 0; i < NUMPOINTS; i++) {
        
        float x = (float)(((points[i].x) * (WIDTH/10.f)) / (TWIDTH)) - (WIDTH/20.f);
        float y = (float)(((points[i].y) * (WIDTH/10.f)) / (TWIDTH)) - (WIDTH/20.f);
        
        sites[i].x = x;
        sites[i].y = y;
        
        sites[i].slopeAngle = points[i].weight;
        
        float preyDensity = frand(1.f) * clamp(sites[i].slopeAngle, 0.2f, 1.f);
        float wolfDensity = frand(1.f) * clamp((1.f - sites[i].slopeAngle), 0.2f, 1.f);
        
        object preyObj = makeShapeObject(TRIANGLE, newV3(1, 1, 0), newV3(0, 0, 1), NULL, GL_DYNAMIC_DRAW, 0);
        object wolfObj = makeShapeObject(ELLIPSOID_2D, newV3(1, 1, 0), newV3(1, 0, 0), NULL, GL_DYNAMIC_DRAW, 2);
        
        preyObj.position = newV3(sites[i].x + 1.f, sites[i].y, 0.1f);
        wolfObj.position = newV3(sites[i].x - 1.f, sites[i].y, 0.11f);
        
        scaleObjTo(&preyObj, newV3(2*preyDensity, 2*preyDensity, 0));
        scaleObjTo(&wolfObj, newV3(2*wolfDensity, 2*wolfDensity, 0));
        
        sites[i].prey = (Prey){preyObj, preyDensity};
        sites[i].wolf = (Wolf){wolfObj, wolfDensity};
        
        sites[i].nextSite = -1;
        
        setNearest(i);
    }
    
}


void timePasses() {
    
    printf("time has passed!\n");
    
    for(int i = 0; i < NUMPOINTS; i++) {
        
        float w = sites[i].wolf.strength;
        float p = sites[i].prey.preyDensity;
        
        float ratio;
        
        if(p <= 0.f) ratio = 99.f;
        else ratio = w / p;
        
        
        float migrate = ratio / (0.9f / 0.1f);
        migrate = clamp(migrate, 0.f, 1.f);
        //float hunt = 1.f - migrate;
        
        bool willMigrate = frand(1.f) <= migrate;
        
        if(willMigrate) {
            
            float wolfMigration = clamp(frand(0.5f), 0.3f, 0.5f) * w;
            wolfMigration = clamp(wolfMigration, wolfMigration, sites[i].wolf.strength);
            
            //sites[i].wolf.strength -= wolfMigration;
            
            int nearest = -1;
            float newRatio = 99999999.f;
            
            for(int j = 0; j < 8; j++) {
                
                if(sites[i].nearest[j] >= 0) {
                    
                    Site checkSite = sites[ sites[i].nearest[j] ];
                    
                    float nearestRatio = checkSite.wolf.strength / checkSite.prey.preyDensity;
                    
                    if(nearestRatio < newRatio)  {
                        newRatio = nearestRatio;
                        nearest = j;
                    }
                }
            }
            
            sites[i].migrationRate = wolfMigration;
            sites[i].nextSite = nearest;
            
            sites[i].prey.preyDensity += p * clamp(frand(0.05f), 0.01f, 0.05f);
            sites[i].prey.preyDensity = clamp(sites[i].prey.preyDensity, 0.f, 1.f);
        }
        
        else {
            
            float huntSuccessChance = 1.f - sites[i].slopeAngle;
            huntSuccessChance = clamp(huntSuccessChance, 0.1f, 0.9f);
            
            bool huntSuccessful = frand(1.f) <= huntSuccessChance;
            
            if(huntSuccessful) {
                
                float huntEffect = w * clamp(frand(0.15f), 0.05f, 0.15f);
                
                sites[i].prey.preyDensity -= huntEffect;
                sites[i].prey.preyDensity = clamp(sites[i].prey.preyDensity, 0.f, 1.f);
                
                sites[i].wolf.strength += huntEffect;
                sites[i].wolf.strength = clamp(sites[i].wolf.strength, 0.f, 1.f);
                
            }
            
            else {
                
                float noHuntEffect = clamp(frand(0.2f), 0.05f, 0.2f);
                
                sites[i].prey.preyDensity += p * clamp(frand(0.05f), 0.01f, 0.05f);
                sites[i].prey.preyDensity = clamp(sites[i].prey.preyDensity, 0.f, 1.f);
                
                sites[i].wolf.strength -= noHuntEffect;
                sites[i].wolf.strength = clamp(sites[i].wolf.strength, 0.f, 1.f);
                
            }
        }
        
        
    }

    
    for(int i = 0; i < NUMPOINTS; i++) {
        
        if(sites[i].nextSite >= 0) {
        
            sites[ sites[i].nextSite ].wolf.strength += sites[i].migrationRate;
            sites[i].wolf.strength -= sites[i].migrationRate;
            
            sites[ sites[i].nextSite ].wolf.strength = clamp(sites[ sites[i].nextSite ].wolf.strength, 0.f, 1.f);
            sites[i].wolf.strength = clamp(sites[i].wolf.strength, 0.f, 1.f);
            
            sites[i].nextSite = -1;

        }
    }
    
    for(int i = 0; i < NUMPOINTS; i++) {
        
        float preyDensity = sites[i].prey.preyDensity;
        float wolfDensity = sites[i].wolf.strength;
    
        scaleObjTo(&sites[i].prey.obj, newV3(2*preyDensity, 2*preyDensity, 0));
        scaleObjTo(&sites[i].wolf.obj, newV3(2*wolfDensity, 2*wolfDensity, 0));
    }
}


void drawSites() {
    
    for(int i = 0; i < NUMPOINTS; i++) {
        drawObject(&sites[i].prey.obj);
        drawObject(&sites[i].wolf.obj);
    }
    
}

void freeSites() {
    
}

