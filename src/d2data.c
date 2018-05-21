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

static CHECK_RESULT d2err d2data_init(d2data* data)
{
	if (data->armorsSet == NULL)
	{
		data->armorsSet = strset_new(100, &strset_hash_default);
	}
	if (data->weaponsSet == NULL)
	{
		data->weaponsSet = strset_new(100, &strset_hash_default);
	}
	if (data->stackablesSet == NULL)
	{
		data->stackablesSet = strset_new(100, &strset_hash_default);
	}
	if (data->armorsSet == NULL || data->weaponsSet == NULL || data->stackablesSet == NULL)
	{
		return D2ERR_OUT_OF_MEMORY;
	}
	return D2ERR_OK;
}

static CHECK_RESULT d2err d2data_load_armors_common(char*** parsed, size_t UNUSED(numRows), d2data* data)
{
	d2err err;
	if ((err = d2data_init(data)) != D2ERR_OK)
	{
		return err;
	}
	int codeCol = d2txt_find_index(parsed, "code");
	if (codeCol < 0)
	{
		return D2ERR_PARSE;
	}

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		char** row = parsed[iRow];
		char* code = row[codeCol];
		if (!code[0]) continue;

		strset_put(data->armorsSet, code);
		i++;
	}
	return D2ERR_OK;
}

CHECK_RESULT d2err d2data_load_armors(const char* txtdata, size_t length, d2data* data)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse(txtdata, length, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2data_load_armors_common(parsed, numRows, data);
	d2txt_destroy(parsed);
	return err;
}

CHECK_RESULT d2err d2data_load_armors_from_file(const char* filename, d2data* data)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse_file(filename, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2data_load_armors_common(parsed, numRows, data);
	d2txt_destroy(parsed);
	return err;
}

static CHECK_RESULT d2err d2data_load_weapons_common(char*** parsed, size_t UNUSED(numRows), d2data* data)
{
	d2err err;
	if ((err = d2data_init(data)) != D2ERR_OK)
	{
		return err;
	}
	int codeCol = d2txt_find_index(parsed, "code");
	int stackableCol = d2txt_find_index(parsed, "stackable");
	if (codeCol < 0 || stackableCol < 0)
	{
		return D2ERR_PARSE;
	}

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
	return D2ERR_OK;
}

CHECK_RESULT d2err d2data_load_weapons(const char* txtdata, size_t length, d2data* data)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse(txtdata, length, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2data_load_weapons_common(parsed, numRows, data);
	d2txt_destroy(parsed);
	return err;
}

CHECK_RESULT d2err d2data_load_weapons_from_file(const char* filename, d2data* data)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse_file(filename, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2data_load_weapons_common(parsed, numRows, data);
	d2txt_destroy(parsed);
	return err;
}

static CHECK_RESULT d2err d2data_load_miscs_common(char*** parsed, size_t UNUSED(numRows), d2data* data)
{
	d2err err;
	if ((err = d2data_init(data)) != D2ERR_OK)
	{
		return err;
	}
	int codeCol = d2txt_find_index(parsed, "code");
	int stackableCol = d2txt_find_index(parsed, "stackable");
	if (codeCol < 0 || stackableCol < 0)
	{
		return D2ERR_PARSE;
	}

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
	return D2ERR_OK;
}

CHECK_RESULT d2err d2data_load_miscs(const char* txtdata, size_t length, d2data* data)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse(txtdata, length, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2data_load_miscs_common(parsed, numRows, data);
	d2txt_destroy(parsed);
	return err;
}

CHECK_RESULT d2err d2data_load_miscs_from_file(const char* filename, d2data* data)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse_file(filename, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2data_load_miscs_common(parsed, numRows, data);
	d2txt_destroy(parsed);
	return err;
}

static CHECK_RESULT d2err d2data_load_itemstats_common(char*** parsed, d2data* data)
{
	int saveBitsCol = d2txt_find_index(parsed, "Save Bits");
	int saveAddCol = d2txt_find_index(parsed, "Save Add");
	int encodeCol = d2txt_find_index(parsed, "Encode");
	int saveParamBitsCol = d2txt_find_index(parsed, "Save Param Bits");
	int charSaveBitsCol = d2txt_find_index(parsed, "CSvBits");
	if (saveBitsCol < 0 || saveAddCol < 0 || encodeCol < 0 || saveParamBitsCol < 0 || charSaveBitsCol < 0)
	{
		return D2ERR_PARSE;
	}

	uint16_t id = 0;
	for (int iRow = 1; parsed[iRow] && id < D2DATA_MAX_ITEMSTATCOST_IDS; iRow++)
	{
		char** row = parsed[iRow];
		char* saveBits = row[saveBitsCol];
		char* saveAdd = row[saveAddCol];
		char* encode = row[encodeCol];
		char* saveParamBits = row[saveParamBitsCol];
		char* charSaveBits = row[charSaveBitsCol];

		data->itemstats[id].id = id;
		data->itemstats[id].saveBits = (uint16_t)(saveBits[0] ? atoi(saveBits) : 0);
		data->itemstats[id].saveAdd = (uint16_t)(saveAdd[0] ? atoi(saveAdd) : 0);
		data->itemstats[id].encode = (uint8_t)(encode[0] ? atoi(encode) : 0);
		data->itemstats[id].saveParamBits = (uint16_t)(saveParamBits[0] ? atoi(saveParamBits) : 0);
		data->itemstats[id].charSaveBits = (uint16_t)(charSaveBits[0] ? atoi(charSaveBits) : 0);

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
	return D2ERR_OK;
}

CHECK_RESULT d2err d2data_load_itemstats(const char* txtdata, size_t length, d2data* data)
{
	d2txt_file parsed;
	d2err err = d2txt_parse(txtdata, length, &parsed, NULL);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2data_load_itemstats_common(parsed, data);
	d2txt_destroy(parsed);
	return err;
}

CHECK_RESULT d2err d2data_load_itemstats_from_file(const char* filename, d2data* data)
{
	d2txt_file parsed;
	d2err err = d2txt_parse_file(filename, &parsed, NULL);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2data_load_itemstats_common(parsed, data);
	d2txt_destroy(parsed);
	return err;
}

void d2data_destroy(d2data* data)
{
	if (data->armorsSet)
	{
		strset_free(data->armorsSet);
		data->armorsSet = NULL;
	}
	if (data->weaponsSet)
	{
		strset_free(data->weaponsSet);
		data->weaponsSet = NULL;
	}
	if (data->stackablesSet)
	{
		strset_free(data->stackablesSet);
		data->stackablesSet = NULL;
	}
}

#include "d2data_default.inc"
#define d2data_default_count(x) (sizeof(x) / sizeof((x)[0]))
CHECK_RESULT d2err d2data_use_default(d2data* data)
{
	d2err err = d2data_init(data);
	if (err != D2ERR_OK)
	{
		return err;
	}
	for (size_t i = 0; i < d2data_default_count(d2data_default_armors); i++)
	{
		strset_put(data->armorsSet, d2data_default_armors[i]);
	}
	for (size_t i = 0; i < d2data_default_count(d2data_default_weapons); i++)
	{
		strset_put(data->weaponsSet, d2data_default_weapons[i]);
	}
	for (size_t i = 0; i < d2data_default_count(d2data_default_stackables); i++)
	{
		strset_put(data->stackablesSet, d2data_default_stackables[i]);
	}
	for (size_t i = 0; i < d2data_default_count(d2data_default_itemstats); i++)
	{
		data->itemstats[i] = d2data_default_itemstats[i];
	}
	return D2ERR_OK;
}