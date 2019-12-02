#include <stdio.h>

#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

int main(int argc, char** argv){
    cl_uint platformCount;
    cl_platform_id* platforms;

    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = new cl_platform_id[platformCount];
    clGetPlatformIDs(platformCount, platforms, NULL);

    char* value;
    size_t valueSize;
    cl_uint deviceCount;
    cl_device_id* devices;
    cl_uint maxComputeUnits;

    for (int i = 0; i < platformCount; i++) {
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        devices = new cl_device_id[deviceCount];
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

        for (int j = 0; j < deviceCount; j++) {
            int k = 0;

            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = new char[valueSize];
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("%d. Device: %s\n", j+1, value);
            free(value);

            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = new char[valueSize];
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf(" %d.%d Hardware version: %s\n", j+1, ++k, value);
            free(value);

            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = new char[valueSize];
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf(" %d.%d Software version: %s\n", j+1, ++k, value);
            free(value);

            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
            value = new char[valueSize];
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
            printf(" %d.%d OpenCL C version: %s\n", j+1, ++k, value);
            free(value);

            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
            printf(" %d.%d Parallel compute units: %d\n", j+1, ++k, maxComputeUnits);

            clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, 0, NULL, &valueSize);
            value = new char[valueSize];
            clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, valueSize, value, NULL);
            printf(" %d.%d Device extensions: %s\n", j+1, ++k, value);
            
            free(value);
        }

        free(devices);
    }

    free(platforms);
    return 0;
}