#include "bitreader.h"
#include "util.h"
#include "minunit.h"
#include <stdbool.h>

#define ITEM_SIMPLE_FILE "data/item_simple.bin"
#define ITEM_ADVANCED_FILE "data/item_advanced.bin"

static unsigned char* simpleItemData;
static unsigned char* advancedItemData;

static void test_setup(void)
{
	size_t bytesRead;
	read_full_file(ITEM_SIMPLE_FILE, &simpleItemData, &bytesRead);
	if (bytesRead == 0)
	{
		fprintf(stderr, "setup: Failed to read %s\n", ITEM_SIMPLE_FILE);
		exit(1);
	}
	read_full_file(ITEM_ADVANCED_FILE, &advancedItemData, &bytesRead);
	if (bytesRead == 0)
	{
		fprintf(stderr, "setup: Failed to read %s\n", ITEM_ADVANCED_FILE);
		exit(1);
	}
}

static void test_teardown(void)
{
	free(simpleItemData);
	free(advancedItemData);
}

MU_TEST(item_data)
{
	bit_reader br = { simpleItemData };
	uint64_t j = read_bits(&br, 8);
	mu_check(j == 'J');
	uint64_t m = read_bits(&br, 8);
	mu_check(m == 'M');
	// offset: 16, unknown
	skip_bits(&br, 4);
	// offset: 20
	bool identified = read_bits(&br, 1);
	mu_check(identified);
	// offset: 21, unknown
	skip_bits(&br, 6);
	// offset: 27
	bool socketed = read_bits(&br, 1);
	mu_check(!socketed);
	// offset 28, unknown
	read_bits(&br, 1);
	// offset 29
	bool isNew = read_bits(&br, 1);
	mu_check(!isNew);
	mu_check(br.bitsRead == 30);
}

MU_TEST(simple_flag)
{
	bit_reader br = { advancedItemData };
	skip_bits(&br, 37);
	bool isSimple = read_bits(&br, 1);
	mu_check(!isSimple);
	mu_check(br.bitsRead == 38);

	bit_reader br2 = { simpleItemData };
	skip_bits(&br2, 37);
	isSimple = read_bits(&br2, 1);
	mu_check(isSimple);
	mu_check(br2.bitsRead == 38);
}

MU_TEST(item_uid)
{
	bit_reader br = { advancedItemData };
	skip_bits(&br, 111);
	uint32_t uid = (uint32_t)read_bits(&br, 32);
	mu_check(uid == (uint32_t)0x103D7536);
}

MU_TEST_SUITE(test_bitreader)
{
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(item_data);
	MU_RUN_TEST(simple_flag);
	MU_RUN_TEST(item_uid);
}

int main(int argc, const char* argv[])
{
	MU_RUN_SUITE(test_bitreader);
	MU_REPORT();
	return 0;
}