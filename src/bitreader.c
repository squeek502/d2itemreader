#include "bitreader.h"

static int read_is_within_bounds(bit_reader* reader, size_t bitsToAdvance)
{
	return reader->bitsRead + bitsToAdvance <= reader->dataSizeBytes * 8;
}

static int within_bounds(bit_reader* reader)
{
	return read_is_within_bounds(reader, 0);
}

static void advance_bits(bit_reader* reader, size_t bitsToAdvance)
{
	reader->bitsRead += bitsToAdvance;
	reader->cursor = within_bounds(reader) ? (reader->bitsRead / 8) : BIT_READER_CURSOR_BEYOND_EOF;
}

void skip_bits(bit_reader* reader, size_t bitsToSkip)
{
	if (reader->cursor != BIT_READER_CURSOR_BEYOND_EOF)
	{
		advance_bits(reader, bitsToSkip);
	}
}

uint64_t read_bits(bit_reader* reader, size_t bitsToRead)
{
	uint64_t n = 0;
	if (read_is_within_bounds(reader, bitsToRead))
	{
		n = read_bits_raw(reader->data, reader->bitsRead, bitsToRead);
	}
	if (reader->cursor != BIT_READER_CURSOR_BEYOND_EOF)
	{
		advance_bits(reader, bitsToRead);
	}
	return n;
}
