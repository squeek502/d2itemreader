#include "d2util.h"
#include <stdio.h>

CHECK_RESULT d2err d2util_read_file(const char* filepath, uint8_t** data_out, size_t* bytesRead)
{
	d2err err;
	FILE* file = fopen(filepath, "rb");
	if (!file)
	{
		err = D2ERR_FILE;
		goto nothing_read;
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	if (size < 0)
	{
		err = D2ERR_FILE;
		goto nothing_read;
	}
	rewind(file);
	*data_out = (uint8_t*)malloc((size_t)size+1);
	if (*data_out == NULL)
	{
		fclose(file);
		err = D2ERR_OUT_OF_MEMORY;
		goto nothing_read;
	}
	(*data_out)[size] = '\0';

	size_t _bytesRead = fread(*data_out, 1, (size_t)size, file);
	fclose(file);

	if (_bytesRead != (size_t)size)
	{
		free(*data_out);
		err = D2ERR_FILE;
		goto nothing_read;
	}

	if (bytesRead) *bytesRead = _bytesRead;
	return D2ERR_OK;

nothing_read:
	*data_out = NULL;
	if (bytesRead) *bytesRead = 0;
	return err;
}
