#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "util.h"

#define D2TXT_MAX_LINE_LEN 4096
#define D2TXT_INITIAL_ARRAY_SIZE 100

// like strtok, but returns "" between consecutive delims
// e.g. d2txt_strsep("a;;b", ";") would give "a", "", "b", NULL
static char* d2txt_strsep(char* str, const char* delims, size_t* out_tokLen)
{
	static char* src = NULL;
	char* p, *ret = 0;

	if (str != NULL)
		src = str;

	if (src == NULL)
		return NULL;

	if ((p = strpbrk(src, delims)) != NULL)
	{
		*p = 0;
		ret = src;
		if (out_tokLen) *out_tokLen = p - ret;
		src = ++p;
	}
	else if (*src)
	{
		ret = src;
		if (out_tokLen) *out_tokLen = strlen(ret);
		src = NULL;
	}

	return ret;
}

// like strtok but non-destructive; instead,  the token is copied to buf and
// returns the pointer to the beginning of the next token, or NULL if
// the current token's length is 0
static const char* d2txt_strtokbuf(const char* str, const char* delims, char* buf, size_t bufSizeInBytes)
{
	static char* src = NULL;

	if (str != NULL)
		src = (char*)str;

	size_t length = strcspn(src, delims);
	assert(length < bufSizeInBytes);
	memcpy(buf, src, length);
	buf[length] = '\0';

	if (length != 0)
	{
		src += length;
		src += strspn(src, delims);
	}
	else
	{
		src = NULL;
	}

	return src;
}

// returns the number of fields inserted (0 or 1)
static size_t d2txt_insert_field(char** row, const char* field, size_t len)
{
	if (field[0] == '\r' || field[0] == '\n') return 0;

	char* str = malloc(len + 1);
	memcpy(str, field, len + 1);

	*row = str;
	return 1;
}

char** d2txt_parse_row(char *line, size_t numFields)
{
	size_t parsedCount = 0;
	char** parsed = malloc((numFields + 1) * sizeof(*parsed));

	size_t tokLen;
	for (const char* tok = d2txt_strsep(line, "\t", &tokLen); tok != NULL && parsedCount < numFields; tok = d2txt_strsep(NULL, "\t", &tokLen))
	{
		parsedCount += d2txt_insert_field(&parsed[parsedCount], tok, tokLen);
	}
	for (; parsedCount < numFields; parsedCount++)
	{
		parsedCount += d2txt_insert_field(&parsed[parsedCount], "", 0);
	}
	parsed[numFields] = NULL;

	return parsed;
}

char** d2txt_parse_header(char *line, size_t *out_numFields)
{
	size_t parsedCount = 0;
	size_t parsedSize = D2TXT_INITIAL_ARRAY_SIZE;
	char** parsed = malloc(parsedSize * sizeof(*parsed));

	size_t tokLen;
	for (const char* tok = d2txt_strsep(line, "\t", &tokLen); tok != NULL; tok = d2txt_strsep(NULL, "\t", &tokLen))
	{
		parsedCount += d2txt_insert_field(&parsed[parsedCount], tok, tokLen);

		if (parsedCount >= parsedSize)
		{
			parsedSize += D2TXT_INITIAL_ARRAY_SIZE;
			parsed = realloc(parsed, parsedSize * sizeof(*parsed));
		}
	}
	parsed[parsedCount] = NULL;

	if (out_numFields)
		*out_numFields = parsedCount;

	return parsed;
}

char*** d2txt_parse(const char *data, size_t length, size_t *out_numRows)
{
	static char line[D2TXT_MAX_LINE_LEN];

	size_t parsedCount = 0;
	size_t parsedSize = D2TXT_INITIAL_ARRAY_SIZE;
	char*** parsed = malloc(parsedSize * sizeof(*parsed));

	if (d2txt_strtokbuf(data, "\r\n", line, D2TXT_MAX_LINE_LEN))
	{
		size_t numFieldsPerRow;
		parsed[parsedCount] = d2txt_parse_header(line, &numFieldsPerRow);
		parsedCount++;

		while (d2txt_strtokbuf(NULL, "\r\n", line, D2TXT_MAX_LINE_LEN))
		{
			parsed[parsedCount] = d2txt_parse_row(line, numFieldsPerRow);
			parsedCount++;

			if (parsedCount >= parsedSize)
			{
				parsedSize += D2TXT_INITIAL_ARRAY_SIZE;
				parsed = realloc(parsed, parsedSize * sizeof(*parsed));
			}
		}
	}
	parsed[parsedCount] = NULL;

	if (out_numRows)
		*out_numRows = parsedCount;

	return parsed;
}

char*** d2txt_parse_file(const char *filename, size_t *out_numRows)
{
	unsigned char* data;
	size_t size;
	read_full_file(filename, &data, &size);
	if (data == NULL)
	{
		return NULL;
	}
	char*** parsed = d2txt_parse(data, size, out_numRows);
	free(data);
	return parsed;
}

void d2txt_destroy_row(char **parsed)
{
	for (char** ptr = parsed; *ptr; ptr++)
	{
		free(*ptr);
	}
	free(parsed);
}

void d2txt_destroy(char ***parsed)
{
	for (char*** ptr = parsed; *ptr; ptr++)
	{
		d2txt_destroy_row(*ptr);
	}
	free(parsed);
}

int d2txt_find_index(char*** parsed, const char* needle)
{
	char** header = *parsed;
	if (header)
	{
		for (int i = 0; header[i]; i++)
		{
			if (strcmp(header[i], needle) == 0)
				return i;
		}
	}
	return -1;
}

/*
int main(int argc, const char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	char*** data = d2txt_parse_file("C:/Users/Ryan/Programming/Diablo/d2grailcheck/SetItems.txt", NULL);
*/
	/*
	for (int i=0; data[i]; i++)
	{
		char** line = data[i];
		printf("line %d: ", i+1);
		int j;
		for (j = 0; line[j]; j++)
		{
			char* field = line[j];
			printf("%s;", field);
		}
		printf("\n");
	}
	*/
/*
	printf("%d\n", d2txt_find_index(data, "index"));
	printf("%d\n", d2txt_find_index(data, "item"));
	printf("%d\n", d2txt_find_index(data, "prop4"));
	printf("%d\n", d2txt_find_index(data, "<<notfound>>"));
	d2txt_destroy(data);
}
*/
