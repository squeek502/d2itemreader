#ifndef D2TXTREADER_H
#define D2TXTREADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "d2util.h"
#include "d2err.h"

typedef char* d2txt_field;
typedef d2txt_field* d2txt_row;
typedef d2txt_row* d2txt_file;

// returns a NULL terminated array of rows, which are in turn
// NULL separated arrays of fields
// parsed[0] is the header row, and all rows afterwards will always have
// the same number of fields as the header row
CHECK_RESULT d2err d2txt_parse_file(const char *filename, d2txt_file* out_parsed, size_t *out_numRows);
CHECK_RESULT d2err d2txt_parse(const char *data, size_t length, d2txt_file* out_parsed, size_t *out_numRows);

// frees the memory used by the return of d2txt_parse_file/d2txt_parse
void d2txt_destroy(d2txt_file parsed);

// parse a row, and get the number of fields
// returns a NULL separated array of field string values
CHECK_RESULT d2err d2txt_parse_header(char *line, d2txt_row* out_parsed, size_t *out_numFields);

// parse a row, while always ending up with the specified number of fields
// returns a NULL separated array of field string values
CHECK_RESULT d2err d2txt_parse_row(char *line, d2txt_row* out_parsed, size_t numFields);

// frees the memory used by a parsed row (header or non-header)
void d2txt_destroy_row(d2txt_row parsed);

// find the index of the header column with the given name
int d2txt_find_index(d2txt_file parsed, const char* needle);

#ifdef __cplusplus
}
#endif

#endif