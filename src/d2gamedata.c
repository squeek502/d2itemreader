#include "d2gamedata.h"
#include "d2txtreader.h"
#include <stdlib.h>
#include <string.h>

bool d2gamedata_is_armor(const d2gamedata* data, const char* itemCode)
{
	return strset_has(data->armorsSet, itemCode);
}

bool d2gamedata_is_weapon(const d2gamedata* data, const char* itemCode)
{
	return strset_has(data->weaponsSet, itemCode);
}

bool d2gamedata_is_stackable(const d2gamedata* data, const char* itemCode)
{
	return strset_has(data->stackablesSet, itemCode);
}

CHECK_RESULT d2err d2gamedata_init_default(d2gamedata* data)
{
	d2err err = d2gamedata_init(data);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_defaults(data);
	if (err != D2ERR_OK)
	{
		d2gamedata_destroy(data);
	}
	return err;
}

CHECK_RESULT d2err d2gamedata_init_files(d2gamedata* data, d2gamedatafiles files)
{
	d2err err = d2gamedata_init(data);
	if (err != D2ERR_OK)
	{
		return err;
	}
	if ((err = d2gamedata_load_armors_from_file(data, files.armorTxtFilepath)) != D2ERR_OK)
	{
		goto err;
	}
	if ((err = d2gamedata_load_weapons_from_file(data, files.weaponsTxtFilepath)) != D2ERR_OK)
	{
		goto err;
	}
	if ((err = d2gamedata_load_miscs_from_file(data, files.miscTxtFilepath)) != D2ERR_OK)
	{
		goto err;
	}
	if ((err = d2gamedata_load_itemstats_from_file(data, files.itemStatCostTxtFilepath)) != D2ERR_OK)
	{
		goto err;
	}
	return D2ERR_OK;

err:
	d2gamedata_destroy(data);
	return err;
}

CHECK_RESULT d2err d2gamedata_init_bufs(d2gamedata* data, d2gamedatabufs bufs)
{
	d2err err = d2gamedata_init(data);
	if (err != D2ERR_OK)
	{
		return err;
	}
	if ((err = d2gamedata_load_armors(data, bufs.armorTxt, bufs.armorTxtSize)) != D2ERR_OK)
	{
		goto err;
	}
	if ((err = d2gamedata_load_weapons(data, bufs.weaponsTxt, bufs.weaponsTxtSize)) != D2ERR_OK)
	{
		goto err;
	}
	if ((err = d2gamedata_load_miscs(data, bufs.miscTxt, bufs.miscTxtSize)) != D2ERR_OK)
	{
		goto err;
	}
	if ((err = d2gamedata_load_itemstats(data, bufs.itemStatCostTxt, bufs.itemStatCostTxtSize)) != D2ERR_OK)
	{
		goto err;
	}
	return D2ERR_OK;

err:
	d2gamedata_destroy(data);
	return err;
}

CHECK_RESULT d2err d2gamedata_init(d2gamedata* data)
{
	memset(data->itemstats, 0, D2DATA_MAX_ITEMSTATCOST_IDS * sizeof(d2gamedata_itemstat));
	data->initState = D2DATA_INIT_STATE_NONE;
	data->armorsSet = strset_new(100, &strset_hash_default);
	if (data->armorsSet == NULL) goto oom_armors;
	data->weaponsSet = strset_new(100, &strset_hash_default);
	if (data->weaponsSet == NULL) goto oom_weapons;
	data->stackablesSet = strset_new(100, &strset_hash_default);
	if (data->stackablesSet == NULL) goto oom_stackables;
	return D2ERR_OK;

oom_stackables:
	strset_free(data->weaponsSet);
oom_weapons:
	strset_free(data->armorsSet);
oom_armors:
	// set these all to NULL so errors gets us into a predictable state
	data->armorsSet = data->weaponsSet = data->stackablesSet = NULL;
	return D2ERR_OUT_OF_MEMORY;
}

static CHECK_RESULT d2err d2gamedata_load_armors_common(d2gamedata* data, d2txt_file parsed, size_t UNUSED(numRows))
{
	int codeCol = d2txt_find_index(parsed, "code");
	if (codeCol < 0)
	{
		d2gamedata_destroy(data);
		return D2ERR_PARSE;
	}

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		d2txt_row row = parsed[iRow];
		d2txt_field code = row[codeCol];
		if (!code[0]) continue;

		if (strset_put(data->armorsSet, code) < 0)
		{
			return D2ERR_OUT_OF_MEMORY;
		}
		i++;
	}
	data->initState |= D2DATA_INIT_STATE_ARMORS;
	return D2ERR_OK;
}

CHECK_RESULT d2err d2gamedata_load_armors(d2gamedata* data, const char* txtdata, size_t length)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse(txtdata, length, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_armors_common(data, parsed, numRows);
	d2txt_destroy(parsed);
	return err;
}

CHECK_RESULT d2err d2gamedata_load_armors_from_file(d2gamedata* data, const char* filename)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse_file(filename, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_armors_common(data, parsed, numRows);
	d2txt_destroy(parsed);
	return err;
}

static CHECK_RESULT d2err d2gamedata_load_weapons_common(d2gamedata* data, d2txt_file parsed, size_t UNUSED(numRows))
{
	int codeCol = d2txt_find_index(parsed, "code");
	int stackableCol = d2txt_find_index(parsed, "stackable");
	if (codeCol < 0 || stackableCol < 0)
	{
		d2gamedata_destroy(data);
		return D2ERR_PARSE;
	}

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		d2txt_row row = parsed[iRow];
		d2txt_field code = row[codeCol];
		if (!code[0]) continue;

		if (strset_put(data->weaponsSet, code) < 0)
		{
			return D2ERR_OUT_OF_MEMORY;
		}
		if (row[stackableCol][0] == '1')
		{
			if (strset_put(data->stackablesSet, code) < 0)
			{
				return D2ERR_OUT_OF_MEMORY;
			}
		}
		i++;
	}
	data->initState |= D2DATA_INIT_STATE_WEAPONS;
	return D2ERR_OK;
}

CHECK_RESULT d2err d2gamedata_load_weapons(d2gamedata* data, const char* txtdata, size_t length)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse(txtdata, length, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_weapons_common(data, parsed, numRows);
	d2txt_destroy(parsed);
	return err;
}

CHECK_RESULT d2err d2gamedata_load_weapons_from_file(d2gamedata* data, const char* filename)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse_file(filename, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_weapons_common(data, parsed, numRows);
	d2txt_destroy(parsed);
	return err;
}

static CHECK_RESULT d2err d2gamedata_load_miscs_common(d2gamedata* data, d2txt_file parsed, size_t UNUSED(numRows))
{
	int codeCol = d2txt_find_index(parsed, "code");
	int stackableCol = d2txt_find_index(parsed, "stackable");
	if (codeCol < 0 || stackableCol < 0)
	{
		d2gamedata_destroy(data);
		return D2ERR_PARSE;
	}

	int i = 0;
	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		d2txt_row row = parsed[iRow];
		d2txt_field code = row[codeCol];
		if (!code[0]) continue;

		if (row[stackableCol][0] == '1')
		{
			if (strset_put(data->stackablesSet, code) < 0)
			{
				return D2ERR_OUT_OF_MEMORY;
			}
		}
		i++;
	}
	data->initState |= D2DATA_INIT_STATE_MISCS;
	return D2ERR_OK;
}

CHECK_RESULT d2err d2gamedata_load_miscs(d2gamedata* data, const char* txtdata, size_t length)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse(txtdata, length, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_miscs_common(data, parsed, numRows);
	d2txt_destroy(parsed);
	return err;
}

CHECK_RESULT d2err d2gamedata_load_miscs_from_file(d2gamedata* data, const char* filename)
{
	size_t numRows;
	d2txt_file parsed;
	d2err err = d2txt_parse_file(filename, &parsed, &numRows);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_miscs_common(data, parsed, numRows);
	d2txt_destroy(parsed);
	return err;
}

static CHECK_RESULT d2err d2gamedata_load_itemstats_common(d2gamedata* data, d2txt_file parsed)
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
		d2txt_row row = parsed[iRow];
		d2txt_field saveBits = row[saveBitsCol];
		d2txt_field saveAdd = row[saveAddCol];
		d2txt_field encode = row[encodeCol];
		d2txt_field saveParamBits = row[saveParamBitsCol];
		d2txt_field charSaveBits = row[charSaveBitsCol];

		data->itemstats[id].id = id;
		data->itemstats[id].saveBits = (uint16_t)(saveBits[0] ? atoi(saveBits) : 0);
		data->itemstats[id].saveAdd = (int16_t)(saveAdd[0] ? atoi(saveAdd) : 0);
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
	data->initState |= D2DATA_INIT_STATE_ITEMSTATS;
	return D2ERR_OK;
}

CHECK_RESULT d2err d2gamedata_load_itemstats(d2gamedata* data, const char* txtdata, size_t length)
{
	d2txt_file parsed;
	d2err err = d2txt_parse(txtdata, length, &parsed, NULL);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_itemstats_common(data, parsed);
	d2txt_destroy(parsed);
	return err;
}

CHECK_RESULT d2err d2gamedata_load_itemstats_from_file(d2gamedata* data, const char* filename)
{
	d2txt_file parsed;
	d2err err = d2txt_parse_file(filename, &parsed, NULL);
	if (err != D2ERR_OK)
	{
		return err;
	}
	err = d2gamedata_load_itemstats_common(data, parsed);
	d2txt_destroy(parsed);
	return err;
}

void d2gamedata_destroy(d2gamedata* data)
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
	memset(data->itemstats, 0, D2DATA_MAX_ITEMSTATCOST_IDS*sizeof(d2gamedata_itemstat));
	data->initState = D2DATA_INIT_STATE_NONE;
}

#include "d2gamedata_default.inc"
#define d2gamedata_default_count(x) (sizeof(x) / sizeof((x)[0]))
CHECK_RESULT d2err d2gamedata_load_defaults(d2gamedata* data)
{
	for (size_t i = 0; i < d2gamedata_default_count(d2gamedata_default_armors); i++)
	{
		if (strset_put(data->armorsSet, d2gamedata_default_armors[i]) < 0)
		{
			return D2ERR_OUT_OF_MEMORY;
		}
	}
	for (size_t i = 0; i < d2gamedata_default_count(d2gamedata_default_weapons); i++)
	{
		if (strset_put(data->weaponsSet, d2gamedata_default_weapons[i]) < 0)
		{
			return D2ERR_OUT_OF_MEMORY;
		}
	}
	for (size_t i = 0; i < d2gamedata_default_count(d2gamedata_default_stackables); i++)
	{
		if (strset_put(data->stackablesSet, d2gamedata_default_stackables[i]) < 0)
		{
			return D2ERR_OUT_OF_MEMORY;
		}
	}
	for (size_t i = 0; i < d2gamedata_default_count(d2gamedata_default_itemstats); i++)
	{
		data->itemstats[i] = d2gamedata_default_itemstats[i];
	}
	data->initState = D2DATA_INIT_STATE_ALL;
	return D2ERR_OK;
}
