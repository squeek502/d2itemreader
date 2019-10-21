#include <stdlib.h>
#include <stdio.h>

#include "d2itemreader.h"

int main(int argc, const char* argv[])
{
	if (argc <= 1)
	{
		printf("usage: %s filename [filename ...]\n", argv[0]);
		return 0;
	}

	// init data
	d2gamedata gameData;
	d2err err = d2gamedata_init_default(&gameData);
	if (err != D2ERR_OK)
	{
		fprintf(stderr, "Failed to initialize default data: %s\n", d2err_str(err));
		return 1;
	}

	for (int i = 1; i <= argc && argv[i]; i++)
	{
		const char* filename = argv[i];
		d2itemreader_stream stream;
		err = d2itemreader_open_file(&stream, filename, &gameData);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to open stream: %s\n", d2err_str(err));
			continue;
		}

		size_t count = 0;
		d2item item;
		while (d2itemreader_next(&stream, &item))
		{
			count++;
		}
		if (stream.err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%zx\n", filename, d2err_str(stream.err), stream.curByte);
			continue;
		}

		printf("%zu items found in %s\n", count, filename);
		d2itemreader_close(&stream);
	}

	d2gamedata_destroy(&gameData);
}
