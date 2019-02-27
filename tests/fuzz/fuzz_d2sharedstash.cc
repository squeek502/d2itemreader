#include "d2itemreader.h"

#ifndef FUZZ_ENTRY
#define FUZZ_ENTRY LLVMFuzzerTestOneInput
#endif

extern "C" int FUZZ_ENTRY(const uint8_t *data, size_t size) {
	d2gamedata gameData;
	d2err err = d2gamedata_init_default(&gameData);
	if (err != D2ERR_OK)
		return 1;
	d2sharedstash stash;
	size_t bytesRead;
	err = d2sharedstash_parse(data, size, &stash, &gameData, &bytesRead);
	if (err == D2ERR_OK)
	{
		d2sharedstash_destroy(&stash);
	}
	d2gamedata_destroy(&gameData);
	return 0;
}
