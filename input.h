#include <stdlib.h>
#include <stdio.h>



gint32 rect(gint32 size,gint32* output)
{
	
	int i = 0;
	for(i=0; i < size/4; i++)
	{
		output[i] = 100;	
	}

	for(i=0; i < size/2; i++)
	{
		output[i + size/4] = 1000;
	}

	for(i=0; i < size/4; i++)
	{
		output[i + size/4 + size/2] = 100;
		if((i + size/4 + size/2) > size)
		{
			return -1;
		}
	}

	return 1;
}
