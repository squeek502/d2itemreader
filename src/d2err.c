#include "d2err.h"

const char* d2err_str(d2err err)
{
	switch (err)
	{
		case D2ERR_OK: return "no error";
		case D2ERR_FILE: return "failed to open file";
		case D2ERR_OUT_OF_MEMORY: return "out of memory";
		case D2ERR_PARSE: return "unknown parse error";
		case D2ERR_PARSE_BAD_HEADER_OR_TAG: return "bad header or tag";
		case D2ERR_PARSE_UNEXPECTED_SOCKETED_ITEM: return "unexpected socket item (child with no parent)";
		case D2ERR_PARSE_TRAILING_BYTES: return "unparsed bytes at end of file";
		case D2ERR_PARSE_NOT_ENOUGH_BYTES: return "not enough bytes in file";
		case D2ERR_PARSE_STRING_TOO_LONG: return "string data too long";
		case D2ERR_PARSE_UNEXPECTED_EOF: return "unexpected end of file";
		case D2ERR_PARSE_UNEXPECTED_NONSOCKETED_ITEM: return "unexpected non-socket item";
		case D2ERR_PARSE_TOO_MANY_STASH_PAGES: return "impossibly large number of stash pages";
		case D2ERR_PARSE_TOO_FEW_STASH_PAGES: return "actual number of stash pages lower than number given in header";
		case D2ERR_DATA_NOT_LOADED: return "game data (.txt files) not loaded";
		case D2ERR_UNSUPPORTED_VERSION: return "unsupported game version";
		case D2ERR_UNKNOWN_FILE_TYPE: return "unknown file type";
		case D2ERR_INTERNAL: return "internal error";
		default: return "unknown error";
	}
}
