#ifndef D2ERR_H_
#define D2ERR_H_

typedef enum d2err {
	D2ERR_OK,
	D2ERR_FILE,
	D2ERR_OUT_OF_MEMORY,
	D2ERR_PARSE,
	D2ERR_PARSE_BAD_HEADER_OR_TAG,
	D2ERR_PARSE_UNEXPECTED_SOCKETED_ITEM,
	D2ERR_PARSE_TRAILING_BYTES,
	D2ERR_PARSE_NOT_ENOUGH_BYTES,
	D2ERR_PARSE_STRING_TOO_LONG
} d2err;

/*
* Get a human readable representation for the error code
*/
const char* d2err_str(d2err err);

#endif