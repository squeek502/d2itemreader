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
	// read_bits_raw will always overflow when the data size is smaller than the read size,
	// but we don't really need to be able to handle this case at all, so just fail
	if (reader->dataSizeBytes < BIT_READER_RAW_READ_SIZE_BYTES)
	{
		reader->cursor = BIT_READER_CURSOR_BEYOND_EOF;
		return 0;
	}

	uint64_t n = 0;
	if (read_is_within_bounds(reader, bitsToRead) && bitsToRead < BIT_READER_RAW_READ_SIZE_BITS)
	{
		// just because the bits are in bounds doesn't mean the full number of bytes that are read in read_bits_raw
		// are, so we still need to handle that case to make sure we don't read out-of-bounds of the data
		if (reader->cursor + BIT_READER_RAW_READ_SIZE_BYTES > reader->dataSizeBytes)
		{
			size_t safeStartBit = (reader->dataSizeBytes - BIT_READER_RAW_READ_SIZE_BYTES) * 8;
			size_t delta = reader->bitsRead - safeStartBit;
			n = read_bits_raw(reader->data, safeStartBit, bitsToRead+delta) >> delta;
		}
		else
			n = read_bits_raw(reader->data, reader->bitsRead, bitsToRead);
	}
	if (reader->cursor != BIT_READER_CURSOR_BEYOND_EOF)
	{
		advance_bits(reader, bitsToRead);
	}
	return n;
}
