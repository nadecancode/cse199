#include "def.h"

#define PROGRAM_FILE "kernel.cl"
#define KERNEL_FUNC "vecAdd"

int main()
{
    unsigned int n = 256;

    float *host_A;
    float *host_B;
    float *host_C;

    cl_mem device_A;
    cl_mem device_B;
    cl_mem device_C;

    size_t size = n * sizeof(float);

    host_A = (float *) malloc(size);
    host_B = (float *) malloc(size);
    host_C = (float *) malloc(size);

    for (int i = 0; i < n; i++) {
        host_A[i] = (float) 1;
        host_B[i] = (float) 1;
    }

    size_t localSize = 64;
    size_t globalSize = ceil(n / (float) localSize) * localSize;

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_int i, err;

    err = clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    FILE *file = fopen(PROGRAM_FILE, "rb");

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char *sourceCode = (char *)malloc(fileSize + 1);
    fread(sourceCode, 1, fileSize, file);
    fclose(file);
    sourceCode[fileSize] = '\0';

    program = clCreateProgramWithSource(context, 1,
                                        (const char **) & sourceCode, NULL, &err);
    queue = clCreateCommandQueue(context, device, 0, &err);
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }

    device_A = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, NULL);
    device_B = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, NULL);
    device_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size, NULL, NULL);

    /* Create a kernel */
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);

    err = clEnqueueWriteBuffer(queue, device_A, CL_TRUE, 0,
                               size, host_A, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(queue, device_B, CL_TRUE, 0,
                                size, host_B, 0, NULL, NULL);

    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &device_A);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &device_B);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &device_C);
    err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);

    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
                                 0, NULL, NULL);
    clFinish(queue);

    clEnqueueReadBuffer(queue, device_C, CL_TRUE, 0,
                        size, host_C, 0, NULL, NULL );

    float sum = 0.0f;

    for(i=0; i<n; i++)
        sum += host_C[i];

    printf("%f", sum);

    clReleaseMemObject(device_A);
    clReleaseMemObject(device_B);
    clReleaseMemObject(device_C);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(host_A);
    free(host_B);
    free(host_C);

    return 0;
}