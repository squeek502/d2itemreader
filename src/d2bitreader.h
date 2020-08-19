#ifndef D2BITREADER_H
#define D2BITREADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

#define BIT_READER_CURSOR_BEYOND_EOF SIZE_MAX
#define BIT_READER_RAW_READ_SIZE_BYTES sizeof(uint64_t)
#define BIT_READER_RAW_READ_SIZE_BITS (BIT_READER_RAW_READ_SIZE_BYTES * 8)

// from https://user.xmission.com/~trevin/DiabloIIv1.09_Item_Format.shtml
#define d2bitreader_read_raw(data,start,size) \
	((*((uint64_t*) &(data)[(start) / 8]) >> ((start) & 7)) & (((uint64_t)1 << (size)) - 1))

typedef struct d2bitreader {
	const uint8_t* const data;
	size_t dataSizeBytes;
	size_t cursor;
	size_t bitCursor;
	size_t bitsRead;
} d2bitreader;

void d2bitreader_skip(d2bitreader* reader, size_t bitsToSkip);
uint64_t d2bitreader_read(d2bitreader* reader, size_t bitsToRead);
size_t d2bitreader_next_byte_pos(d2bitreader* reader);

#ifdef __cplusplus
}
#endif

#endif
