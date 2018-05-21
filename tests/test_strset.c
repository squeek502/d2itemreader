#include "strset.h"
#include "minunit.h"

strset_t* set;

static void test_setup(void)
{
	set = strset_new(5, &strset_hash_default);
}

static void test_teardown(void)
{
	strset_free(set);
}

MU_TEST(test)
{
	strset_put(set, "hello");
	strset_put(set, "a");
	strset_put(set, "a");
	strset_put(set, "a");

	mu_check(strset_has(set, "hello"));
	mu_check(strset_has(set, "hello"));

	mu_check(!strset_has(set, "hellu"));
	mu_check(!strset_has(set, ""));
	mu_check(strset_has(set, "a"));
}

MU_TEST_SUITE(test_strset)
{
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test);
}

int main()
{
	MU_RUN_SUITE(test_strset);
	MU_REPORT();
	return 0;
}
