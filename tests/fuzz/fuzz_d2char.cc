#include "d2itemreader.h"

#ifndef FUZZ_ENTRY
#define FUZZ_ENTRY LLVMFuzzerTestOneInput
#endif

extern "C" int FUZZ_ENTRY(const uint8_t *data, size_t size) {
	d2gamedata gameData;
	d2err err = d2gamedata_init_default(&gameData);
	if (err != D2ERR_OK)
		return 1;
	d2char character;
	size_t bytesRead;
	err = d2char_parse(data, size, &character, &gameData, &bytesRead);
	if (err == D2ERR_OK)
	{
		d2char_destroy(&character);
	}
	d2gamedata_destroy(&gameData);
	return 0;
}
