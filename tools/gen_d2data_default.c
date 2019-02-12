#include <stdlib.h>
#include <stdio.h>

#include "d2data.h"

void dumpstrset(const char* str, void* context)
{
	// context is an int[2] with i as [0] and size as [1]
	size_t* pos = (size_t*)context;
	size_t size = pos[1];
	printf("\"%s\"", str);
	if (pos[0] < size-1)
	{
		printf(",");
	}
	pos[0]++;
}

int main(int argc, const char* argv[])
{
	d2data data = {0};
	d2err err;
	err = d2data_load_armors_from_file(&data, "data/Armor.txt");
	if (err != D2ERR_OK)
	{
		printf("failed to load data/Armor.txt: %s\n", d2err_str(err));
		return 1;
	}
	err = d2data_load_weapons_from_file(&data, "data/Weapons.txt");
	if (err != D2ERR_OK)
	{
		printf("failed to load data/Weapons.txt: %s\n", d2err_str(err));
		return 1;
	}
	err = d2data_load_miscs_from_file(&data, "data/Misc.txt");
	if (err != D2ERR_OK)
	{
		printf("failed to load data/Misc.txt: %s\n", d2err_str(err));
		return 1;
	}
	err = d2data_load_itemstats_from_file(&data, "data/ItemStatCost.txt");
	if (err != D2ERR_OK)
	{
		printf("failed to load data/ItemStatCost.txt: %s\n", d2err_str(err));
		return 1;
	}

	size_t pos[2];

	printf("const char* d2data_default_stackables[] = {");
	pos[0] = 0; pos[1] = strset_count(data.stackablesSet);
	strset_iterate(data.stackablesSet, &dumpstrset, pos);
	printf("};\n");

	printf("const char* d2data_default_weapons[] = {");
	pos[0] = 0; pos[1] = strset_count(data.weaponsSet);
	strset_iterate(data.weaponsSet, &dumpstrset, pos);
	printf("};\n");

	printf("const char* d2data_default_armors[] = {");
	pos[0] = 0; pos[1] = strset_count(data.armorsSet);
	strset_iterate(data.armorsSet, &dumpstrset, pos);
	printf("};\n");

	printf("d2data_itemstat d2data_default_itemstats[] = { ");
	for (int id = 0; id < D2DATA_MAX_ITEMSTATCOST_IDS; id++)
	{
		d2data_itemstat* stat = &data.itemstats[id];
		if (id == stat->id)
		{
			int isLast = (id == D2DATA_MAX_ITEMSTATCOST_IDS - 1) || (data.itemstats[id + 1].id != id + 1);
			printf("{%u, %u, %u, %d, %u, %u, %u}", stat->id, stat->encode, stat->saveBits, stat->saveAdd, stat->saveParamBits, stat->nextInChain, stat->charSaveBits);
			if (!isLast)
				printf(",");
			else
				break;
		}
	}
	printf(" };\n");

	d2data_destroy(&data);
}
