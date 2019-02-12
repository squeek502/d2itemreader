#include "d2itemreader.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	d2gamedata gameData;
	d2err err = d2gamedata_init_default(&gameData);
	if (err != D2ERR_OK)
		return 1;
	d2atmastash stash;
	size_t bytesRead;
	err = d2atmastash_parse(data, size, &stash, &gameData, &bytesRead);
	if (err == D2ERR_OK)
	{
		d2atmastash_destroy(&stash);
	}
	d2gamedata_destroy(&gameData);
	return 0;
}
