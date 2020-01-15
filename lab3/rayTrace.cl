#include "define.h"

__kernel void rayTrace(const int raysLenght, const int spheresLenght, const int resultLenght,
                      const __global float* rays,
                      const __global float8* spheres,
                      __global float* result) {
    const int globalId = get_global_id(0);
    const int localId = get_local_id(0);

    const int rayIndex = globalId * TS * raySize + localId * raySize; 

    float3 origin = { rays[rayIndex], rays[rayIndex + 1], rays[rayIndex + 2] };
    float3 direction = { rays[rayIndex + 3], rays[rayIndex + 4], rays[rayIndex + 5] };

    float3 color = { 0.0f, 0.0f, 0.0f };
    int hits = 0;

    float minDistance = FLT_MAX;
    float3 colorOfNearSphere = { 0.0f, 0.0f, 0.0f };
    bool hitExists = false;

    for(int i = 0; i < spheresLenght; i += sphereSize){
        float3 center = { spheres[i], spheres[i + 1], spheres[i + 2] };
        float doubleRadius = spheres[i + 3] * spheres[i + 3];

        float3 oc = origin - center;
        float a = dot(direction, direction);
        float b = dot(oc, direction) * 2.0f;
        float c = dot(oc, oc) - doubleRadius;

        float discriminant = b * b - 4.0f * a * c;
        if(discriminant < 0.0f){
            continue;
        }

        float sqrtedDiscriminant = sqrt(discriminant);
        float numerator = -b - sqrtedDiscriminant;        
        if(numerator <= 0.0f){
            numerator = -b + sqrtedDiscriminant;
            if(numerator <= 0.0f){
                continue;
            }
        }
        float distance = (numerator / (2.0f * a));

        if(distance >= minDistance){
            continue;
        }

        float3 point = origin + direction * distance;
        float3 N = normalize(point - center);
        float3 light = {0.0f, 10.0f, 0.0f};
        float3 toLight = normalize(light - point);
        float diffuseLightIntensity = max(0.0f, dot(toLight, N));

        if(distance < minDistance){
            hitExists = true;
            minDistance = distance;

            colorOfNearSphere.x = spheres[i + 4] * diffuseLightIntensity; 
            colorOfNearSphere.y = spheres[i + 5] * diffuseLightIntensity; 
            colorOfNearSphere.z = spheres[i + 6] * diffuseLightIntensity;
        }
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    if(hitExists){
        color.x += colorOfNearSphere.x; 
        color.y += colorOfNearSphere.y; 
        color.z += colorOfNearSphere.z;
        hits++;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    
    float k = 1.0f; 
    if(hits > 0){
        k /= (float) hits;
    }

    const int pixel = globalId * resultSize;
    result[pixel    ] = color.x * k;
    result[pixel + 1] = color.y * k;
    result[pixel + 2] = color.z * k;
}