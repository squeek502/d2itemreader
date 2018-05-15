#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif
#include <stdio.h>

#include "d2itemreader.h"

#define SSS_FILE "../save/_LOD_SharedStashSave.sss"
#define D2X_FILE "../save/cool.d2x"
#define D2S_FILE "../save/cool.d2s"
#define D2S_FILE_GOLEM "../save/golem.d2s"
#define D2S_FILE_CLASSIC "../save/classic.d2s"

int main(int argc, const char* argv[])
{
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	d2err err;

	printf("%d %d %d\n", d2filetype_of_file(SSS_FILE), d2filetype_of_file(D2X_FILE), d2filetype_of_file(D2S_FILE));

	err = d2data_use_default(&g_d2data);
	if (err != D2ERR_OK)
	{
		printf("failed to load default data: %s\n", d2err_str(err));
		return 1;
	}

	uint32_t bytesRead;

	d2sharedstash stash;
	err = d2sharedstash_parse(SSS_FILE, &stash, &bytesRead);
	if (err != D2ERR_OK)
	{
		printf("failed to read %s: %s\n", SSS_FILE, d2err_str(err));
		return 1;
	}
	printf("done, %d pages (%u bytes)\n", stash.numPages, bytesRead);
	d2sharedstash_destroy(&stash);

	d2personalstash pstash;
	err = d2personalstash_parse(D2X_FILE, &pstash, &bytesRead);
	if (err != D2ERR_OK)
	{
		printf("failed to read %s: %s\n", D2X_FILE, d2err_str(err));
		return 1;
	}
	printf("done, %d pages (%u bytes)\n", pstash.numPages, bytesRead);
	d2personalstash_destroy(&pstash);

	d2char character;
	err = d2char_parse(D2S_FILE, &character, &bytesRead);
	if (err != D2ERR_OK)
	{
		printf("failed to read %s: %s\n", D2S_FILE, d2err_str(err));
		return 1;
	}
	printf("done, %zu items, %zu corpse items, %zu merc items (%u bytes)\n", character.items.count, character.itemsCorpse.count, character.itemsMerc.count, bytesRead);
	d2char_destroy(&character);

	d2char characterGolem;
	err = d2char_parse(D2S_FILE_GOLEM, &characterGolem, &bytesRead);
	if (err != D2ERR_OK)
	{
		printf("failed to read %s: %s\n", D2S_FILE_GOLEM, d2err_str(err));
		return 1;
	}
	printf("done, %zu items, %zu corpse items, %zu merc items (%u bytes)\n", characterGolem.items.count, characterGolem.itemsCorpse.count, characterGolem.itemsMerc.count, bytesRead);
	d2char_destroy(&characterGolem);

	d2char characterClassic;
	err = d2char_parse(D2S_FILE_CLASSIC, &characterClassic, &bytesRead);
	if (err != D2ERR_OK)
	{
		printf("failed to read %s: %s\n", D2S_FILE_CLASSIC, d2err_str(err));
		return 1;
	}
	printf("done, %zu items, %zu corpse items, %zu merc items (%u bytes)\n", characterClassic.items.count, characterClassic.itemsCorpse.count, characterClassic.itemsMerc.count, bytesRead);
	d2char_destroy(&characterClassic);

	d2data_destroy(&g_d2data);
}
