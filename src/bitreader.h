#ifndef BITREADER_H
#define BITREADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

#define BIT_READER_CURSOR_BEYOND_EOF SIZE_MAX
#define BIT_READER_RAW_READ_SIZE sizeof(uint64_t)

// from https://user.xmission.com/~trevin/DiabloIIv1.09_Item_Format.shtml
#define read_bits_raw(data,start,size) \
	((*((uint64_t*) &(data)[(start) / 8]) >> ((start) & 7)) & (((uint64_t)1 << (size)) - 1))

typedef struct bit_reader {
	const unsigned char* const data;
	size_t dataSizeBytes;
	size_t cursor;
	size_t bitsRead;
} bit_reader;

void skip_bits(bit_reader* reader, size_t bitsToSkip);
uint64_t read_bits(bit_reader* reader, size_t bitsToRead);
uint64_t reverse_bits(uint64_t b, size_t n);

#ifdef __cplusplus
}
#endif

#endif
