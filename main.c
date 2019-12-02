#include <stdio.h>
#include <CL/cl.h>

int main(int argc, char** argv){
    cl_int errcode;
    cl_uint numPlatforms;
    errcode = clGetPlatformIDs(0, NULL, &numPlatforms);
    if(errcode != CL_SUCCESS){
        printf("%d\n", errcode);
        return errcode;
    }

    // cl_platform_id platforms[numPlatforms];
    // errcode = clGetPlatformIDs(numPlatforms, platforms, NULL);
    // if(errcode != CL_SUCCESS){
    //     printf("%d\n", errcode);
    //     return errcode;
    // }

    return 0;
}

