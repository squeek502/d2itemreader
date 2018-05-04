#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <inttypes.h>

#include "bitreader.h"
#include "d2itemreader.h"
#include "d2txtreader.h"

#define SSS_FILE "C:/Users/Ryan/Programming/Diablo/d2grailcheck/_LOD_SharedStashSave.sss"

int main(int argc, const char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	d2data_load_armors("C:/Users/Ryan/Programming/Diablo/diablo2/code/d2_113_data/Armor.txt", &g_d2data);
	d2data_load_weapons("C:/Users/Ryan/Programming/Diablo/diablo2/code/d2_113_data/Weapons.txt", &g_d2data);
	d2data_load_miscs("C:/Users/Ryan/Programming/Diablo/diablo2/code/d2_113_data/Misc.txt", &g_d2data);
	d2data_load_itemstats("C:/Users/Ryan/Programming/Diablo/diablo2/code/d2_113_data/ItemStatCost.txt", &g_d2data);

	d2sharedstash stash;
	uint32_t bytesRead;
	d2sharedstash_parse(SSS_FILE, &stash, &bytesRead);

	printf("done, %d pages\n", stash.numPages);

	d2sharedstash_destroy(&stash);
	d2data_destroy(&g_d2data);
}