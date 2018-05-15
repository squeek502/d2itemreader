#include "d2err.h"

#define D2ERR_MAP(XX) \
	XX(D2ERR_OK, "no error") \
	XX(D2ERR_FILE, "failed to open file") \
	XX(D2ERR_OUT_OF_MEMORY, "out of memory") \
	XX(D2ERR_PARSE, "unknown parse error") \
	XX(D2ERR_PARSE_BAD_HEADER_OR_TAG, "bad header or tag") \
	XX(D2ERR_PARSE_UNEXPECTED_SOCKETED_ITEM, "unexpected socketed item (child with no parent)") \
	XX(D2ERR_PARSE_TRAILING_BYTES, "unparsed bytes at end of file") \
	XX(D2ERR_PARSE_NOT_ENOUGH_BYTES, "not enough bytes in file") \
	XX(D2ERR_PARSE_STRING_TOO_LONG, "string data too long") \

#define D2ERR_STR_GEN(name, msg) case name: return msg;
const char* d2err_str(d2err err)
{
	switch (err)
	{
		D2ERR_MAP(D2ERR_STR_GEN)
	}
	return "unknown error";
}
#undef D2ERR_STR_GEN