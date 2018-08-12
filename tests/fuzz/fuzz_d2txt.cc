#include "d2txtreader.h"
#include <stdint.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse((const char*)data, size, &parsed, &numRows);
	if (err == D2ERR_OK)
	{
		d2txt_destroy(parsed);
	}
	return 0;
}
