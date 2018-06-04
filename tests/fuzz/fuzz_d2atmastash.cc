#include "d2itemreader.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	d2itemreader_init_default();
	d2atmastash stash;
	uint32_t bytesRead;
	d2err err = d2atmastash_parse(data, size, &stash, &bytesRead);
	if (err == D2ERR_OK)
	{
		d2atmastash_destroy(&stash);
	}
	d2itemreader_destroy();
	return 0;
}
