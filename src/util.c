#include "util.h"
#include <stdlib.h>
#include <stdint.h>

void extract_bytes_to_file(FILE* infile, int offset, int bytes, const char* outfile)
{
	long pos = ftell(infile);
	FILE* f = fopen(outfile, "wb");
	unsigned char* buffer = malloc(bytes);
	fseek(infile, offset, SEEK_SET);
	fread(buffer, bytes, 1, infile);
	fwrite(buffer, bytes, 1, f);
	fseek(infile, pos, SEEK_SET);
	fclose(f);
	free(buffer);
}

void read_full_file(const char* filepath, unsigned char** data_out, size_t* bytesRead)
{
	FILE* file = fopen(filepath, "rb");
	if (!file)
	{
		goto nothing_read;
	}

	fseek(file, 0, SEEK_END);
	uint32_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	*data_out = (unsigned char*)malloc(size+1);
	if (*data_out == NULL)
	{
		goto fclose_nothing_read;
	}
	(*data_out)[size] = '\0';

	size_t _bytesRead = fread(*data_out, 1, size, file);
	fclose(file);

	if (bytesRead) *bytesRead = _bytesRead;
	return;

fclose_nothing_read:
	fclose(file);
nothing_read:
	*data_out = NULL;
	if (bytesRead) *bytesRead = 0;
	return;
}

void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		}
		else {
			ascii[i % 16] = '.';
		}
		if ((i + 1) % 8 == 0 || i + 1 == size) {
			printf(" ");
			if ((i + 1) % 16 == 0) {
				printf("|  %s \n", ascii);
			}
			else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				if ((i + 1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i + 1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

void printBits(void const * const ptr, size_t const size)
{
	unsigned char *b = (unsigned char*)ptr;
	unsigned char byte;
	int i, j;

	for (i = (int)size - 1; i >= 0; i--)
	{
		for (j = 7; j >= 0; j--)
		{
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
	}
	puts("");
}