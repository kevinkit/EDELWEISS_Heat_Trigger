/*These are the settings for the convolution*/







gint32 size = 4096; 		//The size of the input vector. 




gint32 i = 0;					//Needed for loops, do not change



/* GPU */
/* Do not change if you do not know what are you doing */

OclPlatform *ocl;
cl_context context;
cl_int errcode;

cl_mem input_gpu;
cl_mem expec_gpu;
cl_mem output_gpu;

cl_program program;
cl_kernel convolution;

cl_command_queue queue;




