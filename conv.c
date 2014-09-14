#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "ocl.h"
#include "input.h"
#include "kernmaker.h"
#include "filewriter.h"
#include "settings.h"

int main(int argc, char* argv[])
{
	gint32 i = 0;
	
	
	char kernel_source[1024];
	
	kernel_maker("new_kernel.cl", size, kernel_source);
	ocl = ocl_new(CL_DEVICE_TYPE_GPU,1);
	context = ocl_get_context(ocl);
	queue = ocl_get_cmd_queues(ocl)[0];

	program = ocl_create_program_from_source(ocl, kernel_source, NULL, &errcode);
	OCL_CHECK_ERROR(errcode);
	convolution = clCreateKernel(program, "conv", &errcode);
	OCL_CHECK_ERROR(errcode);

	
	const size_t SIZE_execution_bit = 2*size;
	const size_t SIZE_input_bit = sizeof(gint32) * (size+1);
	const size_t SIZE_output_bit = sizeof(gint32) * (2*size +1);
	size_t* SIZE_execution_pointer = &SIZE_execution_bit;

	gint32* input= (gint32*) malloc(sizeof(gint32)* (size +1));
	gint32* expec = (gint32*) malloc(sizeof(gint32)*(size +1));

	gint32* output = (gint32*) malloc(sizeof(gint32)*(2*size+ 1));
	
	if(rect(size,expec) == -1)
		{
			exit(1);
		}	


	expec_gpu = clCreateBuffer(context, CL_MEM_READ_ONLY, SIZE_input_bit, NULL, &errcode);
	OCL_CHECK_ERROR(errcode);
	input_gpu = clCreateBuffer(context, CL_MEM_READ_ONLY, SIZE_input_bit, NULL, &errcode);
	OCL_CHECK_ERROR(errcode);
	output_gpu = clCreateBuffer(context, CL_MEM_WRITE_ONLY, SIZE_output_bit, NULL, &errcode);
	OCL_CHECK_ERROR(errcode);


        clEnqueueWriteBuffer(queue, expec_gpu, CL_TRUE, 0, SIZE_input_bit, expec, 0,NULL,NULL);



	//-----here starts the real computation

	if(rect(size, input) == -1){
			exit(1);
			} 

	for(i=0; i < 2*size; i++)
	{
		output[i] = -9999;
	}
 
	clEnqueueWriteBuffer(queue, input_gpu, CL_TRUE, 0, SIZE_input_bit, input, 0, NULL,NULL);


	OCL_CHECK_ERROR(clSetKernelArg(convolution, 0, sizeof(cl_mem), &input_gpu));
	OCL_CHECK_ERROR(clSetKernelArg(convolution, 1, sizeof(cl_mem), &expec_gpu));
	OCL_CHECK_ERROR(clSetKernelArg(convolution, 2, sizeof(cl_mem), &output_gpu));
	
	OCL_CHECK_ERROR(clEnqueueNDRangeKernel(queue, convolution, 1, NULL, SIZE_execution_pointer, NULL,0,NULL,NULL));
	clFinish(queue);
	

	clEnqueueReadBuffer(queue, output_gpu, CL_TRUE, 0, SIZE_output_bit, output, 0, NULL,NULL);
	for(i=0; i < 2*size;i++)
	{
		printf("%d\t", output[i]);
		if(output[i] < 0)
		{
			printf("Some errors occured at point %i \n", i);
			exit(1);
		}
	}


	//Clean
	
        OCL_CHECK_ERROR(clReleaseMemObject(input_gpu));
        OCL_CHECK_ERROR(clReleaseMemObject(expec_gpu));
        OCL_CHECK_ERROR(clReleaseMemObject(output_gpu));

        OCL_CHECK_ERROR(clReleaseKernel(convolution));
        OCL_CHECK_ERROR(clReleaseProgram(program));

	ocl_free(ocl);


	free(input);
	free(output);
	free(expec);

}
