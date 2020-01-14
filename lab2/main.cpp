#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

void printMatrix(int* vector, int size, int rowSize){
    for(int i = 0; i < size; i++)
    {
        printf("%d ", vector[i]);
        if((i + 1) % rowSize == 0){
            printf("\n");    
        }
    }
    printf("\n");
}

int main(void) {
    int M = 8;
    int N = 6;
    int K = 12;

    int aSize = M * K;
    int bSize = K * N;
    int cSize = M * N;

    int *A = (int*)malloc(sizeof(int) * aSize);
    int *B = (int*)malloc(sizeof(int) * bSize);

    srand(time(NULL));
    for(int i = 0; i < aSize; i++){
        A[i] = (rand() % 9) + 1;
    }
    for(int i = 0; i < bSize; i++){
        B[i] = (rand() % 9) + 1;
    }

    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("matrix_multiplication.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, aSize * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, bSize * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, cSize * sizeof(int), NULL, &ret);

    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, aSize * sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, bSize * sizeof(int), B, 0, NULL, NULL);

    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "matrix_multiplication", &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(int), (void *)&M);
    ret = clSetKernelArg(kernel, 1, sizeof(int), (void *)&N);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&K);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&c_mem_obj);
    
    const int TS = 32;
    const size_t local[2] = { TS, TS };
    const size_t global[2] = { M, N };
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global, local, 0, NULL, NULL);

    int *C = (int*)malloc(sizeof(int) * cSize);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, cSize * sizeof(int), C, 0, NULL, NULL);

    printMatrix(A, aSize, K);
    printMatrix(B, bSize, N);
    printMatrix(C, cSize, N);

    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    free(B);
    free(C);
    return 0;
}