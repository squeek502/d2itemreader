#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "d2txtreader.h"

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
		if (out_tokLen) *out_tokLen = (size_t)(p - ret);
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

// like strcspn but works for non-null-terminated strings
size_t d2txt_strcspn(const char *str, size_t len, const char *chars)
{
	size_t i;
	for (i = 0; i < len && str[i]; i++)
	{
		if (strchr(chars, str[i]))
			break;
	}
	return i;
}

// like strspn but works for non-null-terminated strings
size_t d2txt_strspn(const char* str, size_t len, const char* chars)
{
	size_t i;
	for (i = 0; i < len && str[i]; i++)
	{
		if (!strchr(chars, str[i]))
			break;
	}
	return i;
}

// like strtok but non-destructive; instead, the token is copied to buf and
// returns the pointer to the beginning of the next token, or NULL if
// the current token's length is 0
// NOTE: strLen is modified by this function (reduced by the number of characters that were read)
static const char* d2txt_strtokbuf(const char* str, size_t* strLen, const char* delims, char* buf, size_t bufSizeInBytes)
{
	static char* src = NULL;

	if (str != NULL)
		src = (char*)str;

	size_t length = d2txt_strcspn(src, *strLen, delims);
	assert(length < bufSizeInBytes);
	memcpy(buf, src, length);
	buf[length] = '\0';

	if (length != 0)
	{
		src += length;
		*strLen -= length;
		size_t spn = d2txt_strspn(src, *strLen, delims);
		src += spn;
		*strLen -= spn;
	}
	else
	{
		src = NULL;
	}

	return src;
}

// returns the number of fields inserted (0 or 1)
static size_t d2txt_insert_field(d2txt_row row, const d2txt_field field, size_t len)
{
	*row = malloc(len + 1);
	if (*row != NULL)
	{
		memcpy(*row, field, len + 1);
		return 1;
	}
	return 0;
}

CHECK_RESULT d2err d2txt_parse_row(char *line, d2txt_row* out_parsed, size_t numFields)
{
	size_t parsedCount = 0;
	d2txt_row parsed = malloc((numFields + 1) * sizeof(d2txt_row));
	if (parsed == NULL)
	{
		goto oom;
	}

	size_t tokLen;
	for (const char* tok = d2txt_strsep(line, "\t", &tokLen); tok != NULL && parsedCount < numFields; tok = d2txt_strsep(NULL, "\t", &tokLen))
	{
		if (!d2txt_insert_field(&parsed[parsedCount], (const d2txt_field)tok, tokLen))
		{
			goto oom_and_free;
		}
		parsedCount++;
	}
	for (; parsedCount < numFields; parsedCount++)
	{
		if (!d2txt_insert_field(&parsed[parsedCount], "", 0))
		{
			goto oom_and_free;
		}
	}
	parsed[numFields] = NULL;

	*out_parsed = parsed;
	return D2ERR_OK;

oom_and_free:
	free(parsed);
oom:
	*out_parsed = NULL;
	return D2ERR_OUT_OF_MEMORY;
}

CHECK_RESULT d2err d2txt_parse_header(char *line, d2txt_row* out_parsed, size_t *out_numFields)
{
	size_t parsedCount = 0;
	size_t parsedSize = D2TXT_INITIAL_ARRAY_SIZE;
	d2txt_row parsed = malloc(parsedSize * sizeof(*parsed));
	if (parsed == NULL)
	{
		goto oom;
	}

	size_t tokLen;
	for (const char* tok = d2txt_strsep(line, "\t", &tokLen); tok != NULL; tok = d2txt_strsep(NULL, "\t", &tokLen))
	{
		if (!d2txt_insert_field(&parsed[parsedCount], (const d2txt_field)tok, tokLen))
		{
			goto oom_and_free;
		}
		parsedCount++;

		if (parsedCount >= parsedSize)
		{
			parsedSize += D2TXT_INITIAL_ARRAY_SIZE;
			void* tmp = realloc(parsed, parsedSize * sizeof(*parsed));
			if (tmp == NULL)
			{
				goto oom_and_free;
			}
			parsed = tmp;
		}
	}
	parsed[parsedCount] = NULL;

	if (out_numFields)
		*out_numFields = parsedCount;

	*out_parsed = parsed;
	return D2ERR_OK;

oom_and_free:
	free(parsed);
oom:
	*out_parsed = NULL;
	return D2ERR_OUT_OF_MEMORY;
}

CHECK_RESULT d2err d2txt_parse(const char *data, size_t length, d2txt_file* out_parsed, size_t *out_numRows)
{
	d2err err;
	static char line[D2TXT_MAX_LINE_LEN];

	size_t parsedCount = 0;
	size_t parsedSize = D2TXT_INITIAL_ARRAY_SIZE;
	d2txt_file parsed = malloc(parsedSize * sizeof(*parsed));
	if (parsed == NULL)
	{
		err = D2ERR_OUT_OF_MEMORY;
		goto err;
	}

	if (d2txt_strtokbuf(data, &length, "\r\n", line, D2TXT_MAX_LINE_LEN))
	{
		size_t numFieldsPerRow;
		err = d2txt_parse_header(line, &parsed[parsedCount], &numFieldsPerRow);
		if (err != D2ERR_OK)
		{
			goto err_and_free;
		}
		parsedCount++;

		while (d2txt_strtokbuf(NULL, &length, "\r\n", line, D2TXT_MAX_LINE_LEN))
		{
			err = d2txt_parse_row(line, &parsed[parsedCount], numFieldsPerRow);
			if (err != D2ERR_OK)
			{
				goto err_and_free;
			}
			parsedCount++;

			if (parsedCount >= parsedSize)
			{
				parsedSize += D2TXT_INITIAL_ARRAY_SIZE;
				void* tmp = realloc(parsed, parsedSize * sizeof(*parsed));
				if (tmp == NULL)
				{
					err = D2ERR_OUT_OF_MEMORY;
					goto err_and_free;
				}
				parsed = tmp;
			}
		}
	}
	parsed[parsedCount] = NULL;

	if (out_numRows)
		*out_numRows = parsedCount;

	*out_parsed = parsed;
	return D2ERR_OK;

err_and_free:
	free(parsed);
err:
	*out_parsed = NULL;
	return err;
}

CHECK_RESULT d2err d2txt_parse_file(const char *filename, d2txt_file* out_parsed, size_t *out_numRows)
{
	d2err err;
	uint8_t* data;
	size_t size;
	err = d2util_read_file(filename, &data, &size);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2txt_parse((char*)data, size, out_parsed, out_numRows);
	free(data);
	return err;
}

void d2txt_destroy_row(d2txt_row parsed)
{
	for (d2txt_field* ptr = parsed; *ptr; ptr++)
	{
		free(*ptr);
	}
	free(parsed);
}

void d2txt_destroy(d2txt_file parsed)
{
	for (d2txt_row* ptr = parsed; *ptr; ptr++)
	{
		d2txt_destroy_row(*ptr);
	}
	free(parsed);
}

int d2txt_find_index(d2txt_file parsed, const char* needle)
{
	d2txt_row header = *parsed;
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
