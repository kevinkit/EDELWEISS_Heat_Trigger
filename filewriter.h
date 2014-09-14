#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <stdlib.h>
#include <stdio.h>



#include <sys/stat.h>
#include <sys/types.h>


//ONLY WORKS FOR LINUX !

//////////////////////////////////////////////////////////////////////////////////
/* all "add____" functions are adding values to the file */
/* all "over___" functions are overwriting the values in the file ! */
/* openall opens a file and reads out the complete file */
/* openfrac opens a file and reads a specific amount of values out */
/*______________________________________________________________________________*/
/* all functions besides addo or overo are not good for loops due to opening */
/* and closing the file everytime they are used. Better use addo or overo */
/* for opening them only once. */
/* */
/////////////////////////////////////////////////////////////////////////////////


void delete_content(char* filename)
{

	FILE* file;
	file = fopen(filename, "w");
	fclose(file);	
}



FILE* addo(FILE* file, char* filename)
{
	file = fopen(filename, "a+");
	if(file == NULL)
	{
		printf("Error in opening %s", filename);
		exit(1);
	}
	else
	{
		return file;
	}

}


int addowic(char* filename, int value)
{
	FILE* file;
	file = fopen(filename, "a+");
	if(file == NULL)
	{
		printf("Eror in opening %s", filename);
		exit(1);

	}
	else
	{
		fprintf(file, "%d\t", value);
		fclose(file);
		return 1;
	}
}

int addowdc(char* filename, double value)
{
	FILE* file;
	file = fopen(filename, "a+");
	if(file == NULL)
	{
		printf("Eror in opening %s", filename);
		exit(1);

	}
	else
	{
		fprintf(file, "%f\t", value);
		fclose(file);
		return 1;
	}
}




int addowcc(char* filename, char* string)
{

	FILE* file;
	file = fopen(filename, "a+");
	if(file == NULL)
	{
		printf("Eror in opening %s", filename);
		exit(1);

	}
	else
	{
		fprintf(file, "%s", string);
		fclose(file);
		return 1;
	}





}


FILE* overo(FILE* file, char* filename)
{
	file = fopen(filename, "w+");
	if(file == NULL)
	{
		printf("Error in opening %s", filename);
		exit(1);
	}
	else
	{
		return file;
	}

}




int overowic(char* filename, int value)
{
	FILE* file;
	file = fopen(filename, "w+");
	if(file == NULL)
	{
		printf("Eror in opening %s", filename);
		exit(1);

	}
	else
	{
		fprintf(file, "%d\t", value);
		fclose(file);
		return 1;
	}
}

int overowdc(char* filename, double value)
{
	FILE* file;
	file = fopen(filename, "w+");
	if(file == NULL)
	{
		printf("Eror in opening %s", filename);
		exit(1);

	}
	else
	{
		fprintf(file, "%f\t", value);
		fclose(file);
		return 1;
	}
}




int overowcc(char* filename, char* string)
{

	FILE* file;
	file = fopen(filename, "w+");
	if(file == NULL)
	{
		printf("Eror in opening %s", filename);
		exit(1);

	}
	else
	{
		fprintf(file, "%s\t", string);
		fclose(file);
		return 1;
	}


}


int size_bytes(const char* path)
{
	struct stat attribute;
	stat(path, &attribute);

	return (int) attribute.st_size;
}





int openall(const char* path, int* values)
{
	int i = 0;
	struct stat attribute;
	stat(path, &attribute);

	FILE *file;
	file = fopen(path, "r");

	printf("%d\n", (int) attribute.st_size);
	if(file == NULL)
	{
		printf("Error opening file");
		return -1;
	}
	for(; i < (int) (attribute.st_size)/8; i++)
	{
		fscanf(file, "%d", &values[i]);
		//printf("no segmentation fault \n");
	}

	fclose(file);
}

int openfrac(const char* path, int* values, int fraction, int index)
{
	printf("Reading out data will be from %d to %d", index*fraction, (index*fraction) + fraction);
	int i =0;
	struct stat attribute;
	stat(path, &attribute);

	FILE *file;
	file = fopen(path, "r");
	if(attribute.st_size < ((index*fraction) + fraction))
	{
		printf("Index is out of bonds in openfrac");
		return -1;
	}

	int* values_puffer = (int*) malloc((attribute.st_size +1));
	if(file == NULL)
	{
		printf("Error opening file \n");
		return -1;
	}

	for(; i < (int) (attribute.st_size / 8); i++)
	{
		fscanf(file, "%d", &values_puffer[i]);
		printf("no segment fault %i \n", i);
	}

	splitarray(values_puffer, fraction, index, values);
	fclose(file);

}

int splitarray(int *array, int fraction, int index, int* value)
{
	int i = 0;
	for(; i < fraction; i++)
	{
		value[i] = array[i+(fraction*index)];
	}
}






#endif
