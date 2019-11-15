#ifndef D2ERR_H_
#define D2ERR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct d2itemreader_stream d2itemreader_stream; // forward dec

typedef enum d2err {
	D2ERR_OK,
	D2ERR_FILE,
	D2ERR_OUT_OF_MEMORY,
	D2ERR_PARSE,
	D2ERR_PARSE_BAD_HEADER_OR_TAG,
	D2ERR_PARSE_UNEXPECTED_SOCKETED_ITEM,
	D2ERR_PARSE_TRAILING_BYTES,
	D2ERR_PARSE_NOT_ENOUGH_BYTES,
	D2ERR_PARSE_STRING_TOO_LONG,
	D2ERR_PARSE_UNEXPECTED_EOF,
	D2ERR_PARSE_UNEXPECTED_NONSOCKETED_ITEM,
	D2ERR_PARSE_TOO_MANY_STASH_PAGES,
	D2ERR_DATA_NOT_LOADED,
	D2ERR_UNSUPPORTED_VERSION,
	D2ERR_UNKNOWN_FILE_TYPE,
	D2ERR_INTERNAL
} d2err;

/*
* Get a human readable representation for the error code
*/
const char* d2err_str(d2err err);

/*
* Get the byte position of the error in a d2itemreader_stream
*/
size_t d2err_bytepos(d2itemreader_stream* stream);

#ifdef __cplusplus
}
#endif

#endif
