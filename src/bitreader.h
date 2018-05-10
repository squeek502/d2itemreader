#ifndef BITREADER_H
#define BITREADER_H

#include <stdint.h>

typedef struct bit_reader {
	const unsigned char* const data;
	size_t cursor;
	size_t bitsRead;
	uint64_t buffer;
	size_t validBits;
} bit_reader;

void skip_bits(bit_reader* reader, size_t bitsToSkip);
uint64_t read_bits(bit_reader* reader, size_t bitsToRead);
uint64_t reverse_bits(uint64_t b, size_t n);

#endif