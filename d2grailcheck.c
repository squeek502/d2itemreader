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
#define D2X_FILE "C:/Users/Ryan/Programming/Diablo/d2grailcheck/cool.d2x"
#define D2S_FILE "C:/Users/Ryan/Programming/Diablo/d2grailcheck/cool.d2s"
#define D2S_FILE_GOLEM "C:/Users/Ryan/Programming/Diablo/d2grailcheck/golem.d2s"
#define D2S_FILE_CLASSIC "C:/Users/Ryan/Programming/Diablo/d2grailcheck/classic.d2s"

int main(int argc, const char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	printf("%d %d %d\n", d2filetype_of_file(SSS_FILE), d2filetype_of_file(D2X_FILE), d2filetype_of_file(D2S_FILE));

	d2data_load_armors("C:/Users/Ryan/Programming/Diablo/diablo2/code/d2_113_data/Armor.txt", &g_d2data);
	d2data_load_weapons("C:/Users/Ryan/Programming/Diablo/diablo2/code/d2_113_data/Weapons.txt", &g_d2data);
	d2data_load_miscs("C:/Users/Ryan/Programming/Diablo/diablo2/code/d2_113_data/Misc.txt", &g_d2data);
	d2data_load_itemstats("C:/Users/Ryan/Programming/Diablo/diablo2/code/d2_113_data/ItemStatCost.txt", &g_d2data);

	d2sharedstash stash;
	uint32_t bytesRead;
	d2sharedstash_parse(SSS_FILE, &stash, &bytesRead);
	printf("done, %d pages (%u bytes)\n", stash.numPages, bytesRead);
	d2sharedstash_destroy(&stash);

	d2personalstash pstash;
	d2personalstash_parse(D2X_FILE, &pstash, &bytesRead);
	printf("done, %d pages (%u bytes)\n", pstash.numPages, bytesRead);
	d2personalstash_destroy(&pstash);

	d2char character;
	d2char_parse(D2S_FILE, &character, &bytesRead);
	printf("done, %zu items, %zu corpse items, %zu merc items (%u bytes)\n", character.items.count, character.itemsCorpse.count, character.itemsMerc.count, bytesRead);
	d2char_destroy(&character);

	d2char characterGolem;
	d2char_parse(D2S_FILE_GOLEM, &characterGolem, &bytesRead);
	printf("done, %zu items, %zu corpse items, %zu merc items (%u bytes)\n", characterGolem.items.count, characterGolem.itemsCorpse.count, characterGolem.itemsMerc.count, bytesRead);
	d2char_destroy(&characterGolem);

	d2char characterClassic;
	d2char_parse(D2S_FILE_CLASSIC, &characterClassic, &bytesRead);
	printf("done, %zu items, %zu corpse items, %zu merc items (%u bytes)\n", characterClassic.items.count, characterClassic.itemsCorpse.count, characterClassic.itemsMerc.count, bytesRead);
	d2char_destroy(&characterClassic);

	d2data_destroy(&g_d2data);
}