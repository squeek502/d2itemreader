#include <stdlib.h>
#include <stdio.h>

#include "d2itemreader.h"

int main(int argc, const char* argv[])
{
	if (argc <= 1)
	{
		printf("usage: %s filename\n", argv[0]);
		return 0;
	}

	// init data
	d2data_use_default(&g_d2data);

	const char* filename = argv[1];
	enum d2filetype type = d2filetype_of_file(filename);
	if (type == D2FILETYPE_UNKNOWN)
	{
		fprintf(stderr, "Could not determine file format for: %s\n", filename);
		return 1;
	}

	size_t itemCount = 0;
	static uint32_t bytesRead;

	if (type == D2FILETYPE_D2_CHARACTER)
	{
		d2char character;
		d2err err = d2char_parse_file(filename, &character, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%X\n", filename, d2err_str(err), bytesRead);
			return 1;
		}
		itemCount += character.items.count;
		itemCount += character.itemsCorpse.count;
		itemCount += character.itemsMerc.count;
		d2char_destroy(&character);
	}
	else if (type == D2FILETYPE_PLUGY_PERSONAL_STASH)
	{
		d2personalstash stash;
		d2err err = d2personalstash_parse(filename, &stash, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%X\n", filename, d2err_str(err), bytesRead);
			return 1;
		}
		for (size_t i = 0; i<stash.numPages; i++)
		{
			d2stashpage* page = &stash.pages[i];
			itemCount += page->items.count;
		}
		d2personalstash_destroy(&stash);
	}
	else if (type == D2FILETYPE_PLUGY_SHARED_STASH)
	{
		d2sharedstash stash;
		d2err err = d2sharedstash_parse(filename, &stash, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%X\n", filename, d2err_str(err), bytesRead);
			return 1;
		}
		for (size_t i = 0; i<stash.numPages; i++)
		{
			d2stashpage* page = &stash.pages[i];
			itemCount += page->items.count;
		}
		d2sharedstash_destroy(&stash);
	}
	else if (type == D2FILETYPE_ATMA_STASH)
	{
		d2atmastash stash;
		d2err err = d2atmastash_parse(filename, &stash, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%X\n", filename, d2err_str(err), bytesRead);
			return 1;
		}
		itemCount += stash.items.count;
		d2atmastash_destroy(&stash);
	}

	printf("%zu items found in %s\n", itemCount, filename);

	d2data_destroy(&g_d2data);
}
