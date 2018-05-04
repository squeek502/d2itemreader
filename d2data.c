#include "d2data.h"

bool d2data_is_armor(const char* itemCode, const d2data* data)
{
	for (int i = 0; *(data->armors[i].code); i++)
	{
		if (strcmp(itemCode, data->armors[i].code) == 0)
			return true;
	}
	return false;
}

bool d2data_is_weapon(const char* itemCode, const d2data* data)
{
	for (int i = 0; *(data->weapons[i].code); i++)
	{
		if (strcmp(itemCode, data->weapons[i].code) == 0)
			return true;
	}
	return false;
}

bool d2data_is_stackable(const char* itemCode, const d2data* data)
{
	for (int i = 0; *(data->weapons[i].code); i++)
	{
		if (data->weapons[i].stackable && strcmp(itemCode, data->weapons[i].code) == 0)
			return true;
	}
	for (int i = 0; *(data->miscs[i].code); i++)
	{
		if (data->miscs[i].stackable && strcmp(itemCode, data->miscs[i].code) == 0)
			return true;
	}
	return false;
}

void d2data_load_armors(const char* filename, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse_file(filename, &numRows);

	data->armors = malloc(numRows * sizeof(*data->armors));
	int codeCol = d2txt_find_index(parsed, "code");

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		char** row = parsed[iRow];
		char* code = row[codeCol];
		if (!code[0]) continue;

		strcpy_s(data->armors[i].code, sizeof(data->armors[i].code), code);
		i++;
	}
	data->armors[i].code[0] = 0;

	d2txt_destroy_file(parsed);
}

void d2data_load_weapons(const char* filename, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse_file(filename, &numRows);

	data->weapons = malloc(numRows * sizeof(*data->weapons));
	int codeCol = d2txt_find_index(parsed, "code");
	int stackableCol = d2txt_find_index(parsed, "stackable");

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		char** row = parsed[iRow];
		char* code = row[codeCol];
		if (!code[0]) continue;

		strcpy_s(data->weapons[i].code, sizeof(data->weapons[i].code), code);
		data->weapons[i].stackable = row[stackableCol][0] == '1' ? true : false;
		i++;
	}
	data->weapons[i].code[0] = 0;

	d2txt_destroy_file(parsed);
}

void d2data_load_miscs(const char* filename, d2data* data)
{
	size_t numRows;
	char*** parsed = d2txt_parse_file(filename, &numRows);

	data->miscs = malloc(numRows * sizeof(*data->miscs));
	int codeCol = d2txt_find_index(parsed, "code");
	int stackableCol = d2txt_find_index(parsed, "stackable");

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		char** row = parsed[iRow];
		char* code = row[codeCol];
		if (!code[0]) continue;

		strcpy_s(data->miscs[i].code, sizeof(data->miscs[i].code), code);
		data->miscs[i].stackable = row[stackableCol][0] == '1' ? true : false;
		i++;
	}
	data->miscs[i].code[0] = 0;

	d2txt_destroy_file(parsed);
}

void d2data_load_itemstats(const char* filename, d2data* data)
{
	char*** parsed = d2txt_parse_file(filename, NULL);

	int saveBitsCol = d2txt_find_index(parsed, "Save Bits");
	int saveAddCol = d2txt_find_index(parsed, "Save Add");
	int encodeCol = d2txt_find_index(parsed, "Encode");
	int saveParamBitsCol = d2txt_find_index(parsed, "Save Param Bits");

	int id = 0;
	for (int iRow = 1; parsed[iRow] && id < D2DATA_MAX_ITEMSTATCOST_IDS; iRow++)
	{
		char** row = parsed[iRow];
		char* saveBits = row[saveBitsCol];
		char* saveAdd = row[saveAddCol];
		char* encode = row[encodeCol];
		char* saveParamBits = row[saveParamBitsCol];

		data->itemstats[id].id = id;
		data->itemstats[id].saveBits = saveBits[0] ? atoi(saveBits) : 0;
		data->itemstats[id].saveAdd = saveAdd[0] ? atoi(saveAdd) : 0;
		data->itemstats[id].saveParamBits = saveParamBits[0] ? atoi(saveParamBits) : 0;

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

	d2txt_destroy_file(parsed);
}

void d2data_destroy(d2data* data)
{
	if (data->miscs) free(data->miscs);
	if (data->armors) free(data->armors);
	if (data->weapons) free(data->weapons);
}