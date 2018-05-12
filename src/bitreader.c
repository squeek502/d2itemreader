#include "bitreader.h"

#define R2(n)    n,     n + 2*64,     n + 1*64,     n + 3*64
#define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )
static unsigned char lookuptable[256] = { R6(0), R6(2), R6(1), R6(3) };

static unsigned char reverse_byte(unsigned char byte)
{
	return lookuptable[byte];
}

uint64_t reverse_bits(uint64_t b, size_t n)
{
	if (n == 8 && b < 256)
		return lookuptable[b];

	uint64_t d = 0;
	for (unsigned int i = 0; i < n; i++)
	{
		d <<= 1;
		d |= b & 1;
		b >>= 1;
	}
	return d;
}

static void advance_bits(bit_reader* reader, size_t bitsToAdvance)
{
	while (bitsToAdvance > reader->validBits)
	{
		unsigned char byte = *(reader->data + reader->cursor);
		byte = reverse_byte(byte);

		reader->cursor++;
		reader->buffer <<= 8;
		reader->buffer |= (uint64_t)(byte);
		reader->validBits += 8;
	}
	reader->validBits -= bitsToAdvance;
	reader->bitsRead += bitsToAdvance;
}

void skip_bits(bit_reader* reader, size_t bitsToSkip)
{
	advance_bits(reader, bitsToSkip);
}

uint64_t read_bits(bit_reader* reader, size_t bitsToRead)
{
	advance_bits(reader, bitsToRead);
	uint64_t mask = ((uint64_t)1 << bitsToRead) - 1;
	uint64_t n = (reader->buffer >> reader->validBits) & mask;
	return bitsToRead > 1 ? reverse_bits(n, bitsToRead) : n;
}
