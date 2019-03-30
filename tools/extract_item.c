#define D2ITEMREADER_PARSE_ITEM_CALLBACK
#include "d2itemreader.h"
#include "stdio.h"

CHECK_RESULT d2err write_to_file(const char* filepath, const unsigned char* const data, size_t dataSizeBytes)
{
	FILE* file = fopen(filepath, "wb");
	if (!file)
	{
		return D2ERR_FILE;
	}

	size_t bytesWritten = fwrite(data, sizeof(data[0]), dataSizeBytes, file);
	fclose(file);
	if (dataSizeBytes != bytesWritten)
	{
		return D2ERR_FILE;
	}

	return D2ERR_OK;
}

void on_parse_item(d2item* item, const unsigned char* const data, size_t dataSizeBytes)
{
	static size_t itemnum = 1;
	static char filename[100];

	if (item->rarity == D2RARITY_SET && item->setID == 0) {
		sprintf(filename, "extracted_item%03d.d2i", itemnum++);
		d2err err = write_to_file(filename, data, dataSizeBytes);
		if (err == D2ERR_OK)
			printf("wrote %s\n", filename);
		else
			printf("failed to write %s: %s\n", filename, d2err_str(err));
	}
}

int main(int argc, const char* argv[])
{
	if (argc <= 1)
	{
		printf("usage: %s filename\n", argv[0]);
		return 0;
	}

	d2itemreader_set_parse_item_cb(&on_parse_item);

	// init data
	d2gamedata gameData;
	d2err err = d2gamedata_init_default(&gameData);
	if (err != D2ERR_OK)
	{
		fprintf(stderr, "Failed to initialize default data: %s\n", d2err_str(err));
		return 1;
	}

	const char* filename = argv[1];
	enum d2filetype type = d2filetype_of_file(filename);
	if (type == D2FILETYPE_UNKNOWN)
	{
		fprintf(stderr, "Could not determine file format for: %s\n", filename);
		return 1;
	}

	size_t bytesRead;

	if (type == D2FILETYPE_D2_CHARACTER)
	{
		d2char character;
		err = d2char_parse_file(filename, &character, &gameData, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%zx\n", filename, d2err_str(err), bytesRead);
			return 1;
		}
		d2char_destroy(&character);
	}
	else if (type == D2FILETYPE_PLUGY_PERSONAL_STASH)
	{
		d2personalstash stash;
		err = d2personalstash_parse_file(filename, &stash, &gameData, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%zx\n", filename, d2err_str(err), bytesRead);
			return 1;
		}
		d2personalstash_destroy(&stash);
	}
	else if (type == D2FILETYPE_PLUGY_SHARED_STASH)
	{
		d2sharedstash stash;
		err = d2sharedstash_parse_file(filename, &stash, &gameData, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%zx\n", filename, d2err_str(err), bytesRead);
			return 1;
		}
		d2sharedstash_destroy(&stash);
	}
	else if (type == D2FILETYPE_ATMA_STASH)
	{
		d2atmastash stash;
		err = d2atmastash_parse_file(filename, &stash, &gameData, &bytesRead);
		if (err != D2ERR_OK)
		{
			fprintf(stderr, "Failed to parse %s: %s at byte 0x%zx\n", filename, d2err_str(err), bytesRead);
			return 1;
		}
		d2atmastash_destroy(&stash);
	}

	d2gamedata_destroy(&gameData);
}
