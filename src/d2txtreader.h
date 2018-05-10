#ifndef D2TXTREADER_H
#define D2TXTREADER_H

// returns a NULL terminated array of rows, which are in turn
// NULL separated arrays of fields
// parsed[0] is the header row, and all rows afterwards will always have
// the same number of fields as the header row
char*** d2txt_parse_file(const char *filename, size_t *out_numRows);
char*** d2txt_parse(const char *data, size_t length, size_t *out_numRows);

// frees the memory used by the return of d2txt_parse_file/d2txt_parse
void d2txt_destroy(char ***parsed);

// parse a row, and get the number of fields
// returns a NULL separated array of field string values
char** d2txt_parse_header(char *line, size_t *out_numFields);

// parse a row, while always ending up with the specified number of fields
// returns a NULL separated array of field string values
char** d2txt_parse_row(char *line, size_t numFields);

// frees the memory used by a parsed row (header or non-header)
void d2txt_destroy_row(char **parsed);

// find the index of the header column with the given name
int d2txt_find_index(char*** parsed, const char* needle);

#endif