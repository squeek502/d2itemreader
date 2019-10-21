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
		d2filetype type = d2filetype_of_file(filename);
		if (type == D2FILETYPE_UNKNOWN)
		{
			fprintf(stderr, "Could not determine file format for: %s\n", filename);
			continue;
		}

		d2itemlist itemList;
		size_t bytesRead;
		err = d2itemreader_parse_any_file(filename, &itemList, &gameData, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%zx\n", filename, d2err_str(err), bytesRead);
			continue;
		}

		printf("%zu items found in %s\n", itemList.count, filename);
		d2itemlist_destroy(&itemList);
	}

	d2gamedata_destroy(&gameData);
}
