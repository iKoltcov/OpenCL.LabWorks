#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>

#define MAX_SOURCE_SIZE (0x100000)
#include "define.h"

#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

int main(void) {
    int raysLenght = WIDTH * HEIGHT * TS * raySize;
    int spheresLenght = 10 * sphereSize;
    int resultLenght = WIDTH * HEIGHT * resultSize;

    float *rays = (float*)malloc(sizeof(float) * raysLenght);
    float *spheres = (float*)malloc(sizeof(float) * spheresLenght);
    float *result = (float*)malloc(sizeof(float) * resultLenght);

    srand(time(NULL));
    for(int i = 0; i < raysLenght; i += raySize){
        float offsetX = (rand() % 100000) / 100000.0f;
        float offsetY = (rand() % 100000) / 100000.0f;

        rays[i    ] = 0.0f;
        rays[i + 1] = 0.0f;
        rays[i + 2] = 0.0f;

        int pixelIndex = (i / TS);
        int x = pixelIndex % WIDTH;
        int y = pixelIndex / (float) WIDTH;

        float dX = x + offsetX - WIDTH * 0.5f;
        float dY = -(y + offsetY) + HEIGHT * 0.5f;
        float dZ = WIDTH / tanf(FIELD_OF_VIEW * 0.5f);
        
        float max = std::max(dX, std::max(dY, dZ));

        rays[i + 3] = dX / max;
        rays[i + 4] = dY / max;
        rays[i + 5] = dZ / max;
    }

    for(int i = 0; i < spheresLenght; i += sphereSize){
        spheres[i    ] = (rand() % 100) - 50;
        spheres[i + 1] = (rand() % 100) - 50;
        spheres[i + 2] = (rand() % 500) + 10;

        spheres[i + 3] = 1.0f;

        spheres[i + 4] = (rand() % 100000) / 100000.0f;
        spheres[i + 5] = (rand() % 100000) / 100000.0f;
        spheres[i + 6] = (rand() % 100000) / 100000.0f;
    }

    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("rayTrace.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    cl_mem rays_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, raysLenght * sizeof(float), NULL, &ret);
    cl_mem spheres_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, spheresLenght * sizeof(float), NULL, &ret);
    cl_mem result_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, resultLenght * sizeof(float), NULL, &ret);

    ret = clEnqueueWriteBuffer(command_queue, rays_mem_obj, CL_TRUE, 0, raysLenght * sizeof(float), rays, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, spheres_mem_obj, CL_TRUE, 0, spheresLenght * sizeof(float), spheres, 0, NULL, NULL);
    
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
        printf("error: %d\n", ret);
    
    cl_kernel kernel = clCreateKernel(program, "rayTrace", &ret);
        printf("error: %d\n", ret);

    ret = clSetKernelArg(kernel, 0, sizeof(int), (void *)&raysLenght);
    ret = clSetKernelArg(kernel, 1, sizeof(int), (void *)&spheresLenght);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&resultLenght);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&rays_mem_obj);
    ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&spheres_mem_obj);
    ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&result_mem_obj);

    size_t global = raysLenght;
    size_t local = TS;
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global, &local, 0, NULL, NULL);

    ret = clEnqueueReadBuffer(command_queue, result_mem_obj, CL_TRUE, 0, resultLenght * sizeof(float), result, 0, NULL, NULL);
    
    std::ofstream ofs;
    ofs.open("./out.ppm");
    ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (size_t i = 0; i < resultLenght; i++) {
        ofs << (char)(255 * std::max(0.f, std::min(1.f, result[i])));
        if( (i + 1) % WIDTH == 0) {
            ofs << "\n";
        }
    }
    ofs.close();

    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(rays_mem_obj);
    ret = clReleaseMemObject(spheres_mem_obj);
    ret = clReleaseMemObject(result_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(rays);
    free(spheres);
    free(result);
    return 0;
}