#include "d2itemreader.h"
#include "minunit.h"

MU_TEST(nodata)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse_file("data/classic.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_DATA_NOT_LOADED);
}

MU_TEST(classic)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse_file("data/classic.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 7);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(golem)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse_file("data/golem.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 7);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(nomerc)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse_file("data/nomerc.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(character.items.count == 2);
	mu_check(character.itemsCorpse.count == 0);
	mu_check(character.itemsMerc.count == 0);
	d2char_destroy(&character);
}

MU_TEST(badcorpseheader)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse_file("data/badcorpseheader.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_PARSE_BAD_HEADER_OR_TAG);
}

MU_TEST(atma)
{
	d2atmastash stash;
	uint32_t bytesRead;
	d2err err = d2atmastash_parse_file("data/atma.d2x", &stash, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(stash.items.count == 6);
	d2atmastash_destroy(&stash);
}

MU_TEST(plugy_sss)
{
	d2sharedstash stash;
	uint32_t bytesRead;
	d2err err = d2sharedstash_parse_file("data/simple.sss", &stash, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(stash.sharedGold == 10);
	mu_check(stash.numPages == 20);
	d2sharedstash_destroy(&stash);
}

MU_TEST(plugy_d2x)
{
	d2personalstash stash;
	uint32_t bytesRead;
	d2err err = d2personalstash_parse_file("data/simple.d2x", &stash, &bytesRead);
	mu_check(err == D2ERR_OK);
	mu_check(stash.numPages == 1);
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
	uint32_t bytesRead;
	err = d2atmastash_parse(data, dataSizeBytes / 2, &stash, &bytesRead);

	mu_check(err == D2ERR_PARSE_UNEXPECTED_EOF);
	free(data);
}

MU_TEST(d2i)
{
	unsigned char* data;
	size_t dataSizeBytes;
	d2err err = d2util_read_file("data/runeword.d2i", &data, &dataSizeBytes);
	mu_check(err == D2ERR_OK);

	d2itemlist d2i;
	uint32_t bytesRead;
	err = d2itemlist_parse_items(data, dataSizeBytes, 0, &d2i, 1, &bytesRead);
	mu_check(err == D2ERR_OK);

	mu_check(d2i.count == 1);
	mu_check(d2i.items[0].numItemsInSockets == 3);
	mu_check(d2i.items[0].socketedItems.count == 3);

	d2itemlist_destroy(&d2i);
	free(data);
}

MU_TEST_SUITE(test_d2itemreader)
{
	MU_RUN_TEST(nodata);
	d2err err = d2itemreader_init_default();
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
	d2itemreader_destroy();
}

int main()
{
	MU_RUN_SUITE(test_d2itemreader);
	MU_REPORT();
	return 0;
}
