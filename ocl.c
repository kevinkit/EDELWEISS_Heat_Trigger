/*
 *  This file is part of oclkit.
 *
 *  oclkit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  oclkit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ocl.h"

struct OclPlatform {
    cl_platform_id       platform;
    cl_context           context;
    cl_uint              num_devices;
    cl_device_id        *devices;
    cl_command_queue    *cmd_queues;
    int                  own_queues;
};

static const char* opencl_error_msgs[] = {
    "CL_SUCCESS",
    "CL_DEVICE_NOT_FOUND",
    "CL_DEVICE_NOT_AVAILABLE",
    "CL_COMPILER_NOT_AVAILABLE",
    "CL_MEM_OBJECT_ALLOCATION_FAILURE",
    "CL_OUT_OF_RESOURCES",
    "CL_OUT_OF_HOST_MEMORY",
    "CL_PROFILING_INFO_NOT_AVAILABLE",
    "CL_MEM_COPY_OVERLAP",
    "CL_IMAGE_FORMAT_MISMATCH",
    "CL_IMAGE_FORMAT_NOT_SUPPORTED",
    "CL_BUILD_PROGRAM_FAILURE",
    "CL_MAP_FAILURE",
    "CL_MISALIGNED_SUB_BUFFER_OFFSET",
    "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",

    /* next IDs start at 30! */
    "CL_INVALID_VALUE",
    "CL_INVALID_DEVICE_TYPE",
    "CL_INVALID_PLATFORM",
    "CL_INVALID_DEVICE",
    "CL_INVALID_CONTEXT",
    "CL_INVALID_QUEUE_PROPERTIES",
    "CL_INVALID_COMMAND_QUEUE",
    "CL_INVALID_HOST_PTR",
    "CL_INVALID_MEM_OBJECT",
    "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
    "CL_INVALID_IMAGE_SIZE",
    "CL_INVALID_SAMPLER",
    "CL_INVALID_BINARY",
    "CL_INVALID_BUILD_OPTIONS",
    "CL_INVALID_PROGRAM",
    "CL_INVALID_PROGRAM_EXECUTABLE",
    "CL_INVALID_KERNEL_NAME",
    "CL_INVALID_KERNEL_DEFINITION",
    "CL_INVALID_KERNEL",
    "CL_INVALID_ARG_INDEX",
    "CL_INVALID_ARG_VALUE",
    "CL_INVALID_ARG_SIZE",
    "CL_INVALID_KERNEL_ARGS",
    "CL_INVALID_WORK_DIMENSION",
    "CL_INVALID_WORK_GROUP_SIZE",
    "CL_INVALID_WORK_ITEM_SIZE",
    "CL_INVALID_GLOBAL_OFFSET",
    "CL_INVALID_EVENT_WAIT_LIST",
    "CL_INVALID_EVENT",
    "CL_INVALID_OPERATION",
    "CL_INVALID_GL_OBJECT",
    "CL_INVALID_BUFFER_SIZE",
    "CL_INVALID_MIP_LEVEL",
    "CL_INVALID_GLOBAL_WORK_SIZE"
};

const char*
ocl_strerr (int error)
{
    int index = 0;

    if (error >= -14)
        index = -error;
    else if (error <= -30 && error >= -64)
        index = -error-15;

    return opencl_error_msgs[index];
}

static void
transfer_error (cl_int src, cl_int *dst)
{
    if (dst != NULL)
        *dst = src;
}

static char *
ocl_read_program (const char *filename)
{
    FILE *fp;
    char *buffer;
    size_t length;
    size_t buffer_length;

    if ((fp = fopen(filename, "r")) == NULL)
        return NULL;

    fseek (fp, 0, SEEK_END);
    length = ftell (fp);
    rewind (fp);

    buffer = malloc (length + 1);
    buffer[length] = '\0';

    if (buffer == NULL) {
        fclose(fp);
        return NULL;
    }

    buffer_length = fread (buffer, 1, length, fp);
    fclose(fp);

    if (buffer_length != length) {
        free (buffer);
        buffer = NULL;
    }

    return buffer;
}

OclPlatform *
ocl_new (cl_device_type type,
         int create_queues)
{
    OclPlatform *ocl;
    cl_int errcode;

    ocl = malloc (sizeof(OclPlatform));

    OCL_CHECK_ERROR (clGetPlatformIDs (1, &ocl->platform, NULL));
    OCL_CHECK_ERROR (clGetDeviceIDs (ocl->platform, type, 0, NULL, &ocl->num_devices));

    ocl->devices = malloc (ocl->num_devices * sizeof(cl_device_id));
    OCL_CHECK_ERROR (clGetDeviceIDs (ocl->platform, type, ocl->num_devices, ocl->devices, NULL));

    ocl->context = clCreateContext (NULL, ocl->num_devices, ocl->devices, NULL, NULL, &errcode);
    OCL_CHECK_ERROR (errcode);

    if (create_queues) {
        ocl->own_queues = 1;
        ocl->cmd_queues = malloc (ocl->num_devices * sizeof(cl_command_queue));
		cl_uint i = 0;
        for (; i < ocl->num_devices; i++) {
            ocl->cmd_queues[i] = clCreateCommandQueue (ocl->context, ocl->devices[i], 0, &errcode);
            OCL_CHECK_ERROR (errcode);
        }
    }
    else
        ocl->own_queues = 0;

    return ocl;
}

void
ocl_free (OclPlatform *ocl)
{
    if (ocl->own_queues) {
		cl_uint i = 0;
        for (; i < ocl->num_devices; i++)
            OCL_CHECK_ERROR (clReleaseCommandQueue (ocl->cmd_queues[i]));

        free (ocl->cmd_queues);
    }

    OCL_CHECK_ERROR (clReleaseContext (ocl->context));

    free (ocl->devices);
    free (ocl);
}

char *
ocl_get_platform_info (OclPlatform *ocl,
                       cl_platform_info param)
{
    size_t size;
    char *result;

    OCL_CHECK_ERROR (clGetPlatformInfo (ocl->platform, param, 0, NULL, &size));
    result = malloc (size);
    OCL_CHECK_ERROR (clGetPlatformInfo (ocl->platform, param, size, result, NULL));
    return result;
}

cl_program
ocl_create_program_from_source (OclPlatform *ocl,
                                const char *source,
                                const char *options,
                                cl_int *errcode)
{
    cl_int tmp_err;
    cl_program program;
    
    program = clCreateProgramWithSource (ocl->context, 1, (const char **) &source, NULL, &tmp_err);

    if (tmp_err != CL_SUCCESS) {
        transfer_error (tmp_err, errcode);
        return NULL;
    }

    tmp_err = clBuildProgram (program, ocl->num_devices, ocl->devices, options, NULL, NULL);

    if (tmp_err != CL_SUCCESS) {
        size_t log_size;
        char* log;

        transfer_error (tmp_err, errcode);

        OCL_CHECK_ERROR (clGetProgramBuildInfo (program, ocl->devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size));
        log = malloc (log_size * sizeof(char));

        OCL_CHECK_ERROR (clGetProgramBuildInfo (program, ocl->devices[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL));
        fprintf (stderr, "\n** Error building program. Build log:\n%s\n", log);
        free (log);
        return NULL;
    }

    *errcode = CL_SUCCESS;

    return program;
}

cl_program
ocl_create_program_from_file (OclPlatform *ocl,
                              const char *filename,
                              const char *options,
                              cl_int *errcode)
{
    char *source;
    cl_program program;

    source = ocl_read_program (filename);

    if (source == NULL)
        return NULL;

    program = ocl_create_program_from_source (ocl, source, options, errcode);
    free(source);
    return program;
}

cl_context
ocl_get_context (OclPlatform *ocl)
{
    assert (ocl != NULL);
    return ocl->context;
}

int
ocl_get_num_devices (OclPlatform *ocl)
{
    assert (ocl != NULL);
    return ocl->num_devices;
}

cl_device_id *
ocl_get_devices (OclPlatform *ocl)
{
    assert (ocl != NULL);
    return ocl->devices;
}

cl_command_queue *
ocl_get_cmd_queues (OclPlatform *ocl)
{
    assert (ocl != NULL);
    return ocl->cmd_queues;
}
