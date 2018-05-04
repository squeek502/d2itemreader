#include "bitreader.h"

static unsigned char reverse_byte(unsigned char byte)
{
	unsigned char reversed = 0;
	for (int i = 0; i < 8; i++)
	{
		reversed <<= 1;
		reversed |= byte & 1;
		byte >>= 1;
	}
	return reversed;
}

uint64_t reverse_bits(uint64_t b, size_t n)
{
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
	uint64_t n = (reader->buffer >> (reader->validBits)) & ((1 << bitsToRead) - 1);
	return reverse_bits(n, bitsToRead);
}
