#include "d2itemreader.h"
#include "minunit.h"

MU_TEST(classic)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse("data/classic.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_OK);
	d2char_destroy(&character);
}

MU_TEST(golem)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse("data/golem.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_OK);
	d2char_destroy(&character);
}

MU_TEST(nomerc)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse("data/nomerc.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_OK);
	d2char_destroy(&character);
}

MU_TEST(badcorpseheader)
{
	d2char character;
	uint32_t bytesRead;
	d2err err = d2char_parse("data/badcorpseheader.d2s", &character, &bytesRead);
	mu_check(err == D2ERR_PARSE_BAD_HEADER_OR_TAG);
}

MU_TEST_SUITE(test_d2itemreader)
{
	MU_RUN_TEST(classic);
	MU_RUN_TEST(golem);
	MU_RUN_TEST(nomerc);
	MU_RUN_TEST(badcorpseheader);
}

int main(int argc, const char* argv[])
{
	d2data_use_default(&g_d2data);
	MU_RUN_SUITE(test_d2itemreader);
	MU_REPORT();
	d2data_destroy(&g_d2data);
	return 0;
}