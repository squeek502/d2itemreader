#ifndef D2ITEMREADER_UTIL_H
#define D2ITEMREADER_UTIL_H

#include <stdio.h>

void extract_bytes_to_file(FILE* infile, int offset, int bytes, const char* outfile);
void read_full_file(const char* filepath, unsigned char** data_out, size_t* bytesRead);
void DumpHex(const void* data, size_t size);
void printBits(void const * const ptr, size_t const size);

#endif