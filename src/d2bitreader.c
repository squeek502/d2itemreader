#include "d2bitreader.h"

static int read_is_within_bounds(d2bitreader* reader, size_t bitsToAdvance)
{
	return reader->bitCursor + bitsToAdvance <= reader->dataSizeBytes * 8;
}

static int within_bounds(d2bitreader* reader)
{
	return read_is_within_bounds(reader, 0);
}

static void advance_bits(d2bitreader* reader, size_t bitsToAdvance)
{
	reader->bitsRead += bitsToAdvance;
	reader->bitCursor += bitsToAdvance;
	reader->cursor = within_bounds(reader) ? (reader->bitCursor / 8) : BIT_READER_CURSOR_BEYOND_EOF;
}

void d2bitreader_skip(d2bitreader* reader, size_t bitsToSkip)
{
	if (reader->cursor != BIT_READER_CURSOR_BEYOND_EOF)
	{
		advance_bits(reader, bitsToSkip);
	}
}

uint64_t d2bitreader_read(d2bitreader* reader, size_t bitsToRead)
{
	// d2bitreader_read_raw will always overflow when the data size is smaller than the read size,
	// but we don't really need to be able to handle this case at all, so just fail
	if (reader->dataSizeBytes < BIT_READER_RAW_READ_SIZE_BYTES)
	{
		reader->cursor = BIT_READER_CURSOR_BEYOND_EOF;
		return 0;
	}

	uint64_t n = 0;
	if (read_is_within_bounds(reader, bitsToRead) && bitsToRead < BIT_READER_RAW_READ_SIZE_BITS)
	{
		// just because the bits are in bounds doesn't mean the full number of bytes that are read in d2bitreader_read_raw
		// are, so we still need to handle that case to make sure we don't read out-of-bounds of the data
		if (reader->cursor + BIT_READER_RAW_READ_SIZE_BYTES > reader->dataSizeBytes)
		{
			size_t safeStartBit = (reader->dataSizeBytes - BIT_READER_RAW_READ_SIZE_BYTES) * 8;
			size_t delta = reader->bitCursor - safeStartBit;
			if (bitsToRead + delta < BIT_READER_RAW_READ_SIZE_BITS)
			{
				n = d2bitreader_read_raw(reader->data, safeStartBit, bitsToRead + delta) >> delta;
			}
		}
		else
			n = d2bitreader_read_raw(reader->data, reader->bitCursor, bitsToRead);
	}
	if (reader->cursor != BIT_READER_CURSOR_BEYOND_EOF)
	{
		advance_bits(reader, bitsToRead);
	}
	return n;
}

size_t d2bitreader_next_byte_pos(d2bitreader* reader)
{
	return reader->cursor + (reader->bitCursor % 8 != 0);
}
