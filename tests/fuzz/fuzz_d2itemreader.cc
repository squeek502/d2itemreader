#include "d2itemreader.h"

#ifndef FUZZ_ENTRY
#define FUZZ_ENTRY LLVMFuzzerTestOneInput
#endif

extern "C" int FUZZ_ENTRY(const uint8_t *data, size_t size) {
	d2gamedata gameData;
	d2err err = d2gamedata_init_default(&gameData);
	if (err != D2ERR_OK)
		return 1;
	d2itemlist itemList;
	size_t bytesRead;
	err = d2itemreader_parse_any(data, size, &itemList, &gameData, &bytesRead);
	if (err == D2ERR_OK)
	{
		d2itemlist_destroy(&itemList);
	}
	d2gamedata_destroy(&gameData);
	return 0;
}
