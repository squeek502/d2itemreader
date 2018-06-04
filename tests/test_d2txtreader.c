#include "d2txtreader.h"
#include "minunit.h"

MU_TEST(zero)
{
	d2txt_file parsed;
	size_t numRows;
	d2err err = d2txt_parse(NULL, 0, &parsed, &numRows);
	mu_check(err == D2ERR_OK);
	mu_check(numRows == 0);
	d2txt_destroy(parsed);
}

MU_TEST(books)
{
	d2txt_file parsed;
	size_t numRows;
	d2err err = d2txt_parse_file("data/Books.txt", &parsed, &numRows);
	mu_check(err == D2ERR_OK);
	mu_check(numRows == 4);
	d2txt_destroy(parsed);
}

MU_TEST_SUITE(test_d2txtreader)
{
	MU_RUN_TEST(zero);
	MU_RUN_TEST(books);
}

int main()
{
	MU_RUN_SUITE(test_d2txtreader);
	MU_REPORT();
	return 0;
}
