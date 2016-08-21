#pragma once

#include <stdio.h>
#include <stdlib.h>

char* tqReadFile(const char* fileName, long* size)
{
	FILE* file = fopen(fileName, "rb");
	if (!file) {
		if (size) {
			*size = 0;
		}
		return NULL;
	}
	
	fseek(file, 0, SEEK_END);
	const long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	char* fileData = (char*) malloc(fileSize + 1);
	fread(fileData, fileSize, 1, file);
	fclose(file);
	fileData[fileSize] = 0;
	
	if (size) {
		*size = fileSize;
	}
	return fileData;
}

void tqFreeFile(char* fileData)
{
	free(fileData);
}