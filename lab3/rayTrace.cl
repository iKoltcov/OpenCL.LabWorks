#include "define.h"

__kernel void rayTrace(const int raysLenght, const int spheresLenght, const int resultLenght,
                      const __global float* rays,
                      const __global float* spheres,
                      __global float* result) {
                          
    float3 color = { 0.0f, 0.0f, 0.0f };

    const int rayIndex = get_global_id(0);
    float3 origin = { rays[rayIndex], rays[rayIndex + 1], rays[rayIndex + 2] };
    float3 direction = { rays[rayIndex + 3], rays[rayIndex + 4], rays[rayIndex + 5] };

    for(int i = 0; i < spheresLenght; i += sphereSize){
        float3 center = { spheres[i], spheres[i + 1], spheres[i + 2] };
        float doubleRadius = spheres[i + 3] * spheres[i + 3];

        float3 L = center - origin;
        float tca = dot(L, direction);
        float d2 = dot(L, L) - dot(tca, tca);

        if(d2 > doubleRadius){
            continue;
        }

        float thc = sqrt(doubleRadius - d2);

        float t0 = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) {
            t0 = t1;
        }
        
        if (t0 < 0) {
            continue;
        }

        color.x = spheres[i + 4]; 
        color.y = spheres[i + 5]; 
        color.z = spheres[i + 6];
        break;
    }

    const int pixel = get_group_id(0);
    result[pixel    ] = color.x;
    result[pixel + 1] = color.y;
    result[pixel + 2] = color.z;
}