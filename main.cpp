#include <stdio.h>

#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

typedef struct {
    int Id;
    char* Name;
} Parameter;

char* toString(cl_device_type deviceType){
    switch (deviceType)
    {
        case 0 : return (char*)"CL_DEVICE_TYPE_DEFAULT";
        case 1 : return (char*)"CL_DEVICE_TYPE_CPU";
        case 2 : return (char*)"CL_DEVICE_TYPE_GPU";
        case 3 : return (char*)"CL_DEVICE_TYPE_ACCELERATOR";
        case 4 : return (char*)"CL_DEVICE_TYPE_CUSTOM";
        case 0xFFFFFFFF : return (char*)"CL_DEVICE_TYPE_ALL";

        default: return (char*)"Unexpected type";
    }
}

int main(int argc, char** argv){
    cl_uint platformCount;
    cl_platform_id* platforms;

    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = new cl_platform_id[platformCount];
    clGetPlatformIDs(platformCount, platforms, NULL);

    char* charValues;
    size_t valueSize;

    cl_uint intValue;
    cl_bool boolValue;

    Parameter platformParameters[5] = {
        { CL_PLATFORM_NAME, (char*)"CL_PLATFORM_NAME" },
        { CL_PLATFORM_VENDOR, (char*)"CL_PLATFORM_VENDOR" },
        { CL_PLATFORM_VERSION, (char*)"CL_PLATFORM_VERSION" },
        { CL_PLATFORM_PROFILE, (char*)"CL_PLATFORM_PROFILE" },
        { CL_PLATFORM_EXTENSIONS, (char*)"CL_PLATFORM_EXTENSIONS" }
    }; 

    Parameter deviceCharParameters[] = {
        { CL_DEVICE_NAME, (char*)"CL_DEVICE_NAME" },
        { CL_DEVICE_VERSION, (char*)"CL_DEVICE_VERSION" },
        { CL_DEVICE_OPENCL_C_VERSION, (char*)"CL_DEVICE_OPENCL_C_VERSION" },
        { CL_DEVICE_EXTENSIONS, (char*)"CL_DEVICE_EXTENSIONS" },
        { CL_DRIVER_VERSION, (char*)"CL_DRIVER_VERSION" }
    };

    Parameter deviceBoolParameters[] = {
        { CL_DEVICE_AVAILABLE, (char*)"CL_DEVICE_AVAILABLE" },
        { CL_DEVICE_COMPILER_AVAILABLE, (char*)"CL_DEVICE_COMPILER_AVAILABLE" },
    };

    Parameter deviceNumberParameters[] = {
        { CL_DEVICE_MAX_COMPUTE_UNITS, (char*)"CL_DEVICE_MAX_COMPUTE_UNITS" },
        { CL_DEVICE_GLOBAL_MEM_SIZE, (char*)"CL_DEVICE_GLOBAL_MEM_SIZE" },
        { CL_DEVICE_LOCAL_MEM_SIZE, (char*)"CL_DEVICE_LOCAL_MEM_SIZE" },
        { CL_DEVICE_LOCAL_MEM_TYPE, (char*)"CL_DEVICE_LOCAL_MEM_TYPE" },
        { CL_DEVICE_MAX_WORK_GROUP_SIZE, (char*)"CL_DEVICE_MAX_WORK_GROUP_SIZE" }
    };

    for (int i = 0; i < platformCount; i++) {
        
        for(int j = 0; j < sizeof(platformParameters)/sizeof(*platformParameters); j++){
            clGetPlatformInfo(platforms[i], platformParameters[j].Id, 0, NULL, &valueSize);
            charValues = new char[valueSize];
            clGetPlatformInfo(platforms[i], platformParameters[j].Id, valueSize, charValues, NULL);
            printf("%s: %s\n", platformParameters[j].Name, charValues );
            free(charValues);
        }

        cl_uint deviceCount;
        cl_device_id* devices;
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        devices = new cl_device_id[deviceCount];
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

        for(int j = 0; j < deviceCount; j++){
            for(int k = 0; k < sizeof(deviceCharParameters)/sizeof(*deviceCharParameters); k++){
                clGetDeviceInfo(devices[j], deviceCharParameters[k].Id, 0, NULL, &valueSize);
                charValues = new char[valueSize];
                clGetDeviceInfo(devices[j], deviceCharParameters[k].Id, valueSize, charValues, NULL);
                printf(k != 0 ? "\t\t%s: %s\n" : "\t%s: %s\n", + deviceCharParameters[k].Name, charValues);
                delete[] charValues;    
            }
            for(int k = 0; k < sizeof(deviceNumberParameters)/sizeof(*deviceNumberParameters); k++){
                clGetDeviceInfo(devices[j], deviceNumberParameters[k].Id, sizeof(intValue), &intValue, NULL);
                printf("\t\t%s: %d\n", deviceNumberParameters[k].Name, intValue);
            }
            for(int k = 0; k < sizeof(deviceBoolParameters)/sizeof(*deviceBoolParameters); k++){
                clGetDeviceInfo(devices[j], deviceBoolParameters[k].Id, sizeof(boolValue), &boolValue, NULL);
                printf("\t\t%s: %s\n", deviceBoolParameters[k].Name, boolValue ? (char*)"True" : (char*)"False");
            }      

            cl_device_type deviceType;
            clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, NULL);
            printf("\t\tCL_DEVICE_TYPE: %s\n", toString(deviceType));

            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(intValue), &intValue, NULL);
            printf("\t\t%s: %d\n", "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS", intValue);

            size_t* sizeValues = new size_t[intValue];
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * intValue, sizeValues, NULL);
            printf("\t\tCL_DEVICE_MAX_WORK_ITEM_SIZES: ");
            for(int k = 0; k < intValue; k++){
                printf(k + 1 == intValue ? "%lu \n" : "%lu / ", sizeValues[k]);           
            }
            delete[] sizeValues;
        }

        delete[] devices;
    }

    delete[] platforms;
    return 0;
}