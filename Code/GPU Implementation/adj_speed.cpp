// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//// dist_mgmt.cpp
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#pragma comment(lib, "OpenCL.lib")
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "C:/Program Files (x86)/AMD APP SDK/3.0/include/CL/cl.h"

#define MAX_SOURCE_SIZE 8192


int main(void) {
    printf("Truck Platooning Simulation\n");

    // Create the four input
    const int LIST_SIZE = 4;

    // Truck information arrays
    float* velocities = (float*)malloc(sizeof(float) * LIST_SIZE);
    float* adjusted_speeds = (float*)malloc(sizeof(float) * LIST_SIZE);
    float communication_gain = 0.1f; // Replace 0.1f with your desired communication gain value


    velocities[0] = 30.0f;
    velocities[1] = 35.0f;
    velocities[2] = 28.0f;
    velocities[3] = 32.0f;

    // Load the platooning kernel source code into the array source_str
    FILE* fp;
    char* source_str;
    size_t source_size;

    if (fopen_s(&fp, "adj_speed_kernel.cl", "r") != 0) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }

    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);
    printf("Kernel loading done\n");

    // Get platform and device information
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;

    cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
    cl_platform_id* platforms = NULL;
    platforms = (cl_platform_id*)malloc(ret_num_platforms * sizeof(cl_platform_id));

    ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);

    ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1,
        &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each variable
    cl_mem velocities_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
        LIST_SIZE * sizeof(float), NULL, &ret);
    cl_mem adjusted_speeds_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
        LIST_SIZE * sizeof(float), NULL, &ret);

    // Copy the truck velocities to their respective memory buffer
    ret = clEnqueueWriteBuffer(command_queue, velocities_mem_obj, CL_TRUE, 0,
        LIST_SIZE * sizeof(float), velocities, 0, NULL, NULL);

    printf("Before kernel execution\n");

    // Create a program from the kernel source code
    cl_program program = clCreateProgramWithSource(context, 1,
        (const char**)&source_str, (const size_t*)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    printf("After building\n");

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "platooning_communication", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&velocities_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&adjusted_speeds_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(float), (void*)&communication_gain);

    // Execute the OpenCL kernel
    size_t global_item_size = LIST_SIZE;
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
        &global_item_size, NULL, 0, NULL, NULL);

    printf("After kernel execution\n");

    // Read the memory buffer (adjusted speeds) on the device to the local variable
    ret = clEnqueueReadBuffer(command_queue, adjusted_speeds_mem_obj, CL_TRUE, 0,
        LIST_SIZE * sizeof(float), adjusted_speeds, 0, NULL, NULL);

    // Display the output to the screen
    printf("Truck Velocities:\n");
    for (int i = 0; i < LIST_SIZE; i++)
        printf("%.2f\t", velocities[i]);

    printf("\nAdjusted Speeds:\n");
    for (int i = 0; i < LIST_SIZE; i++)
        printf("%.2f\t", adjusted_speeds[i]);

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(velocities_mem_obj);
    ret = clReleaseMemObject(adjusted_speeds_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(velocities);
    free(adjusted_speeds);

    return 0;
}
