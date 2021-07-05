#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <linux/limits.h>
#define MAX_PATH PATH_MAX
#endif

#include "d2gamedata.h"

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
	if (argc <= 1)
	{
		fprintf(stderr, "usage: %s data_dir\n", argv[0]);
		return 1;
	}

	const char* data_dir = argv[1];
	char filepath[MAX_PATH];

	d2gamedata data;
	d2err err = d2gamedata_init(&data);
	if (err != D2ERR_OK)
	{
		printf("failed to init d2gamedata struct: %s\n", d2err_str(err));
		return 1;
	}
	sprintf(filepath, "%s/%s", data_dir, "Armor.txt");
	err = d2gamedata_load_armors_from_file(&data, filepath);
	if (err != D2ERR_OK)
	{
		printf("failed to load %s: %s\n", filepath, d2err_str(err));
		return 1;
	}
	sprintf(filepath, "%s/%s", data_dir, "Weapons.txt");
	err = d2gamedata_load_weapons_from_file(&data, filepath);
	if (err != D2ERR_OK)
	{
		printf("failed to load %s: %s\n", filepath, d2err_str(err));
		return 1;
	}
	sprintf(filepath, "%s/%s", data_dir, "Misc.txt");
	err = d2gamedata_load_miscs_from_file(&data, filepath);
	if (err != D2ERR_OK)
	{
		printf("failed to load %s: %s\n", filepath, d2err_str(err));
		return 1;
	}
	sprintf(filepath, "%s/%s", data_dir, "ItemStatCost.txt");
	err = d2gamedata_load_itemstats_from_file(&data, filepath);
	if (err != D2ERR_OK)
	{
		printf("failed to load %s: %s\n", filepath, d2err_str(err));
		return 1;
	}

	size_t pos[2];

	printf("const char* d2gamedata_default_stackables[] = {");
	pos[0] = 0; pos[1] = strset_count(data.stackablesSet);
	strset_iterate(data.stackablesSet, &dumpstrset, pos);
	printf("};\n");

	printf("const char* d2gamedata_default_weapons[] = {");
	pos[0] = 0; pos[1] = strset_count(data.weaponsSet);
	strset_iterate(data.weaponsSet, &dumpstrset, pos);
	printf("};\n");

	printf("const char* d2gamedata_default_armors[] = {");
	pos[0] = 0; pos[1] = strset_count(data.armorsSet);
	strset_iterate(data.armorsSet, &dumpstrset, pos);
	printf("};\n");

	printf("d2gamedata_itemstat d2gamedata_default_itemstats[] = { ");
	for (int id = 0; id < D2DATA_MAX_ITEMSTATCOST_IDS; id++)
	{
		d2gamedata_itemstat* stat = &data.itemstats[id];
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

	d2gamedata_destroy(&data);
}
