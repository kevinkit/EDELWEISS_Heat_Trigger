#ifndef KERNMAKER_H
#define KERNMAKER_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filewriter.h"
void kernel_maker(char* filename, int size, char* content)
{
	delete_content(filename);


	


	char kernel[1024] = "__kernel void conv(__global int* input_1, __global int* input_2, __global int* output)"
			"{"
			"	int i = 0;"
			"	int id = get_global_id(0);"
			"	output[id] = 0;"
			"	if(id >= 4096)"
			"	{"
			"		for(i=id; i < 2*4096; i++)"
			"		{"
			"			output[id] += input_1[i - 4096] * input_2[id - i + 4096];"
			"			if((i-4096) > 4096 || (id - i +4096 > 4096))"
			"			{"
			"				output[id]=-5000;"
			"			}"
			"		}"
			"	}"
			"	else"
			"	{"
			"		for(i=0; i < id+1 ; i++)"
			"		{"
			"			output[id] += input_1[i]*input_2[id -i];"
			"			if((i > 4096 )|| (id - i) > 4096)"
			"			{"
			"				output[id] = - 4999;"
			"			}"
			"		}"
			"	}"
			"}";


	sprintf(kernel, "__kernel void conv(global int* input_1, global int* input_2, global int* output)"
                        "{"
                        "       int id = get_global_id(0);"
                        "       int i = 0;"
                        "       output[id] = 0;"
                        "       if(id >= %d)"
                        "       {"
                        "               for(i=id; i < 2*%d; i++)"
                        "               {"
                        "                       output[id] += input_1[i - %d] * input_2[id - i + %d];"
                        "                       if((i-%d) > %d || (id - i +%d > %d))"
                        "                       {"
                        "                               output[id]=-5000;"
                        "                       }"
                        "               }"
                        "       }"
                        "       else"
                        "       {"
                        "               for(i=0; i < id+1 ; i++)"
                        "               {"
                        "                       output[id] += input_1[i]*input_2[id -i];"
                        "                       if((i > %d)|| (id - i) > %d)"
                        "                       {"
                        "                               output[id] = - 4999;"
                        "                       }"
                        "               }"
                        "       }"
                        "}", size, size, size , size, size, size , size, size,size,size);

	
	
	printf("%s", kernel);
	addowcc(filename, kernel);
	sprintf(content, kernel);
}



#endif 
