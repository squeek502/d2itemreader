#include "d2itemreader.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	d2itemreader_init_default();
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse(data, size, &character, &bytesRead);
	if (err == D2ERR_OK)
	{
		d2char_destroy(&character);
	}
	d2itemreader_destroy();
	return 0;
}
