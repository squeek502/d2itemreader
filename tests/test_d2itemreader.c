#include "d2itemreader.h"
#include "minunit.h"

d2gamedata gameData;

MU_TEST(nodata)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/classic.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_DATA_NOT_LOADED);
}

MU_TEST(classic)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/classic.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 7);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(golem)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/golem.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 7);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(nomerc)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/nomerc.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 2);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(badcorpseheader)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/badcorpseheader.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_PARSE_BAD_HEADER_OR_TAG);
}

MU_TEST(atma)
{
	d2atmastash stash;
	size_t bytesRead;
	d2err err = d2atmastash_parse_file("data/atma.d2x", &stash, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(stash.items.count == 6);
	d2atmastash_destroy(&stash);
}

MU_TEST(plugy_sss)
{
	d2sharedstash stash;
	size_t bytesRead;
	d2err err = d2sharedstash_parse_file("data/simple.sss", &stash, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(stash.sharedGold == 10);
	mu_check(stash.numPages == 20);
	for (size_t i = 0; i < stash.numPages; i++)
	{
		d2stashpage* page = &stash.pages[i];
		mu_check(page->pageNum == i + 1);
		d2itemlist* items = &stash.pages[i].items;
		if (page->pageNum == 1)
			mu_check(items->count == 4);
		else if (page->pageNum == 20)
			mu_check(items->count == 1);
		else
			mu_check(items->count == 0);
	}
	d2sharedstash_destroy(&stash);
}

MU_TEST(plugy_d2x)
{
	d2personalstash stash;
	size_t bytesRead;
	d2err err = d2personalstash_parse_file("data/simple.d2x", &stash, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(stash.numPages == 1);
	mu_check(stash.pages[0].items.count == 24);
	d2personalstash_destroy(&stash);
}

MU_TEST(unexpected_eof)
{
	d2err err;

	unsigned char* data;
	size_t dataSizeBytes;
	err = d2util_read_file("data/atma.d2x", &data, &dataSizeBytes);
	mu_check(err == D2ERR_OK);

	d2atmastash stash;
	size_t bytesRead;
	err = d2atmastash_parse(data, dataSizeBytes / 2, &stash, &gameData, &bytesRead);

	mu_check(err == D2ERR_PARSE_UNEXPECTED_EOF);
	free(data);
}

MU_TEST(d2i)
{
	const char* filename = "data/runeword.d2i";
	mu_check(d2filetype_of_file(filename) == D2FILETYPE_D2_ITEM);

	d2item d2i;
	size_t bytesRead;
	d2err err = d2item_parse_file(filename, &d2i, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);

	mu_check(d2i.numItemsInSockets == 3);
	mu_check(d2i.socketedItems.count == 3);

	d2item_destroy(&d2i);
}

MU_TEST(v107)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/d2v107.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_UNSUPPORTED_VERSION);
}

MU_TEST(v108)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/d2v108.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_UNSUPPORTED_VERSION);
}

MU_TEST(v109b)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/d2v109b.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_UNSUPPORTED_VERSION);
}

MU_TEST(v109d)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/d2v109d.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_UNSUPPORTED_VERSION);
}

MU_TEST(v110f)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/d2v110f.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 8);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(v111b)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/d2v111b.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 8);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(v112a)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/d2v112a.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 8);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(v113c)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/d2v113c.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 4);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(properties1)
{
	d2item item;
	size_t bytesRead;
	d2err err = d2item_parse_file("data/properties_test1.d2i", &item, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(item.magicProperties.count == 3);

	d2itemprop* prop1 = &item.magicProperties.properties[0];
	mu_check(prop1->id == 308);
	mu_check(prop1->numParams == 1);
	mu_check(prop1->params[0] == 25);

	d2itemprop* prop2 = &item.magicProperties.properties[1];
	mu_check(prop2->id == 112);
	mu_check(prop2->numParams == 1);
	mu_check(prop2->params[0] == 126);

	d2itemprop* prop3 = &item.magicProperties.properties[2];
	mu_check(prop3->id == 89);
	mu_check(prop3->numParams == 1);
	mu_check(prop3->params[0] == -4);

	d2item_destroy(&item);
}

MU_TEST(ear)
{
	d2item item;
	size_t bytesRead;
	d2err err = d2item_parse_file("data/ear.d2i", &item, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(item.isEar);
	mu_check(item.ear.classID == 4);
	mu_check(item.ear.level == 34);
	mu_check(strcmp(item.ear.name, "abcdejdjkdjkjdj") == 0);
	d2item_destroy(&item);
}

MU_TEST(earchar)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/ear.d2s", &character, &gameData, &bytesRead);
	mu_check(err == D2ERR_OK);
	d2char_destroy(&character);
}

MU_TEST(sets)
{
	d2char character;
	size_t bytesRead;
	d2err err = d2char_parse_file("data/sets.d2s", &character, &gameData, &bytesRead);
	mu_check(character.items.count == 11);
	mu_check(err == D2ERR_OK);
	d2char_destroy(&character);
}

MU_TEST(string_too_long)
{
	d2personalstash stash;
	size_t bytesRead;
	d2err err = d2personalstash_parse_file("data/err-string-too-long.d2x", &stash, &gameData, &bytesRead);
	mu_check(err == D2ERR_PARSE_STRING_TOO_LONG);
	mu_check(bytesRead == 0x14);
}

MU_TEST_SUITE(test_d2itemreader)
{
	MU_RUN_TEST(nodata);
	d2err err = d2gamedata_init_default(&gameData);
	mu_check(err == D2ERR_OK);
	MU_RUN_TEST(classic);
	MU_RUN_TEST(golem);
	MU_RUN_TEST(nomerc);
	MU_RUN_TEST(badcorpseheader);
	MU_RUN_TEST(atma);
	MU_RUN_TEST(plugy_sss);
	MU_RUN_TEST(plugy_d2x);
	MU_RUN_TEST(unexpected_eof);
	MU_RUN_TEST(d2i);
	MU_RUN_TEST(v107);
	MU_RUN_TEST(v108);
	MU_RUN_TEST(v109b);
	MU_RUN_TEST(v109d);
	MU_RUN_TEST(v110f);
	MU_RUN_TEST(v111b);
	MU_RUN_TEST(v112a);
	MU_RUN_TEST(v113c);
	MU_RUN_TEST(properties1);
	MU_RUN_TEST(ear);
	MU_RUN_TEST(earchar);
	MU_RUN_TEST(sets);
	MU_RUN_TEST(string_too_long);
	d2gamedata_destroy(&gameData);
}

int main()
{
	MU_RUN_SUITE(test_d2itemreader);
	MU_REPORT();
	return minunit_fail;
}
