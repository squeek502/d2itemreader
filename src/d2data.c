#include "d2data.h"
#include "d2txtreader.h"
#include <stdlib.h>
#include <string.h>

bool d2data_is_armor(const char* itemCode, const d2data* data)
{
	return strset_has(data->armorsSet, itemCode);
}

bool d2data_is_weapon(const char* itemCode, const d2data* data)
{
	return strset_has(data->weaponsSet, itemCode);
}

bool d2data_is_stackable(const char* itemCode, const d2data* data)
{
	return strset_has(data->stackablesSet, itemCode);
}

static void d2data_load_armors_common(char*** parsed, size_t numRows, d2data* data)
{
	if (data->armorsSet == NULL)
	{
		data->armorsSet = strset_new(100, &strset_hash_default);
	}
	int codeCol = d2txt_find_index(parsed, "code");

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		char** row = parsed[iRow];
		char* code = row[codeCol];
		if (!code[0]) continue;

		strset_put(data->armorsSet, code);
		i++;
	}
}

void d2data_load_armors(const char* txtdata, size_t length, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse(txtdata, length, &numRows);
	d2data_load_armors_common(parsed, numRows, data);
	d2txt_destroy(parsed);
}

void d2data_load_armors_from_file(const char* filename, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse_file(filename, &numRows);
	d2data_load_armors_common(parsed, numRows, data);
	d2txt_destroy(parsed);
}

static void d2data_load_weapons_common(char*** parsed, size_t numRows, d2data* data)
{
	if (data->weaponsSet == NULL)
	{
		data->weaponsSet = strset_new(100, &strset_hash_default);
	}
	if (data->stackablesSet == NULL)
	{
		data->stackablesSet = strset_new(100, &strset_hash_default);
	}
	int codeCol = d2txt_find_index(parsed, "code");
	int stackableCol = d2txt_find_index(parsed, "stackable");

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		char** row = parsed[iRow];
		char* code = row[codeCol];
		if (!code[0]) continue;

		strset_put(data->weaponsSet, code);
		if (row[stackableCol][0] == '1')
		{
			strset_put(data->stackablesSet, code);
		}
		i++;
	}
}

void d2data_load_weapons(const char* txtdata, size_t length, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse(txtdata, length, &numRows);
	d2data_load_weapons_common(parsed, numRows, data);
	d2txt_destroy(parsed);
}

void d2data_load_weapons_from_file(const char* filename, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse_file(filename, &numRows);
	d2data_load_weapons_common(parsed, numRows, data);
	d2txt_destroy(parsed);
}

static void d2data_load_miscs_common(char*** parsed, size_t numRows, d2data* data)
{
	if (data->stackablesSet == NULL)
	{
		data->stackablesSet = strset_new(100, &strset_hash_default);
	}
	int codeCol = d2txt_find_index(parsed, "code");
	int stackableCol = d2txt_find_index(parsed, "stackable");

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		char** row = parsed[iRow];
		char* code = row[codeCol];
		if (!code[0]) continue;

		if (row[stackableCol][0] == '1')
		{
			strset_put(data->stackablesSet, code);
		}
		i++;
	}
}

void d2data_load_miscs(const char* txtdata, size_t length, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse(txtdata, length, &numRows);
	d2data_load_miscs_common(parsed, numRows, data);
	d2txt_destroy(parsed);
}

void d2data_load_miscs_from_file(const char* filename, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse_file(filename, &numRows);
	d2data_load_miscs_common(parsed, numRows, data);
	d2txt_destroy(parsed);
}

static void d2data_load_itemstats_common(char*** parsed, d2data* data)
{
	int saveBitsCol = d2txt_find_index(parsed, "Save Bits");
	int saveAddCol = d2txt_find_index(parsed, "Save Add");
	int encodeCol = d2txt_find_index(parsed, "Encode");
	int saveParamBitsCol = d2txt_find_index(parsed, "Save Param Bits");
	int charSaveBitsCol = d2txt_find_index(parsed, "CSvBits");

	int id = 0;
	for (int iRow = 1; parsed[iRow] && id < D2DATA_MAX_ITEMSTATCOST_IDS; iRow++)
	{
		char** row = parsed[iRow];
		char* saveBits = row[saveBitsCol];
		char* saveAdd = row[saveAddCol];
		char* encode = row[encodeCol];
		char* saveParamBits = row[saveParamBitsCol];
		char* charSaveBits = row[charSaveBitsCol];

		data->itemstats[id].id = id;
		data->itemstats[id].saveBits = saveBits[0] ? atoi(saveBits) : 0;
		data->itemstats[id].saveAdd = saveAdd[0] ? atoi(saveAdd) : 0;
		data->itemstats[id].saveParamBits = saveParamBits[0] ? atoi(saveParamBits) : 0;
		data->itemstats[id].charSaveBits = charSaveBits[0] ? atoi(charSaveBits) : 0;

		// these specific ids are melded together with the next ID, but there's no signifier of this in the
		// .txt file, so we have to hardcode this list
		if (id == 17 || id == 48 || id == 50 || id == 52 || id == 54 || id == 55 || id == 57 || id == 58)
		{
			data->itemstats[id].nextInChain = id + 1;
		}
		else
		{
			data->itemstats[id].nextInChain = 0;
		}

		id++;
	}
}

void d2data_load_itemstats(const char* txtdata, size_t length, d2data* data)
{
	char*** parsed = d2txt_parse(txtdata, length, NULL);
	d2data_load_itemstats_common(parsed, data);
	d2txt_destroy(parsed);
}

void d2data_load_itemstats_from_file(const char* filename, d2data* data)
{
	char*** parsed = d2txt_parse_file(filename, NULL);
	d2data_load_itemstats_common(parsed, data);
	d2txt_destroy(parsed);
}

void d2data_destroy(d2data* data)
{
	if (data->armorsSet) strset_free(data->armorsSet);
	if (data->weaponsSet) strset_free(data->weaponsSet);
	if (data->stackablesSet) strset_free(data->stackablesSet);
}
