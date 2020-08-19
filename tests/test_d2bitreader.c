#include "d2bitreader.h"
#include "d2util.h"
#include "minunit.h"
#include <stdbool.h>

#define ITEM_SIMPLE_FILE "data/item_simple.bin"
#define ITEM_ADVANCED_FILE "data/item_advanced.bin"

static uint8_t* simpleItemData;
static size_t simpleItemDataSize;
static uint8_t* advancedItemData;
static size_t advancedItemDataSize;

static void test_setup(void)
{
	d2err err;
	err = d2util_read_file(ITEM_SIMPLE_FILE, &simpleItemData, &simpleItemDataSize);
	if (err != D2ERR_OK || simpleItemDataSize == 0)
	{
		fprintf(stderr, "setup: Failed to read %s\n", ITEM_SIMPLE_FILE);
		exit(1);
	}
	err = d2util_read_file(ITEM_ADVANCED_FILE, &advancedItemData, &advancedItemDataSize);
	if (err != D2ERR_OK || advancedItemDataSize == 0)
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
	d2bitreader br = { simpleItemData, simpleItemDataSize };
	uint64_t j = d2bitreader_read(&br, 8);
	mu_check(j == 'J');
	uint64_t m = d2bitreader_read(&br, 8);
	mu_check(m == 'M');
	// offset: 16, unknown
	d2bitreader_skip(&br, 4);
	// offset: 20
	bool identified = d2bitreader_read(&br, 1);
	mu_check(identified);
	// offset: 21, unknown
	d2bitreader_skip(&br, 6);
	// offset: 27
	bool socketed = d2bitreader_read(&br, 1);
	mu_check(!socketed);
	// offset 28, unknown
	d2bitreader_read(&br, 1);
	// offset 29
	bool isNew = d2bitreader_read(&br, 1);
	mu_check(!isNew);
	mu_check(br.bitsRead == 30);
}

MU_TEST(item_code)
{
	d2bitreader br = { simpleItemData, simpleItemDataSize };
	d2bitreader_skip(&br, 76);
	char c = (char)d2bitreader_read(&br, 8);
	mu_check(c == 'r');
	c = (char)d2bitreader_read(&br, 8);
	mu_check(c == '1');
	c = (char)d2bitreader_read(&br, 8);
	mu_check(c == '1');
	c = (char)d2bitreader_read(&br, 8);
	mu_check(c == ' ');
}

MU_TEST(large_read)
{
	d2bitreader br = { simpleItemData, simpleItemDataSize };
	uint64_t d = d2bitreader_read(&br, 1);
	mu_check(d == 0);
	br.cursor = 0; br.bitsRead = 0; br.bitCursor = 0;
	d = d2bitreader_read(&br, 2);
	mu_check(d == 2);
	br.cursor = 0; br.bitsRead = 0; br.bitCursor = 0;
	d = d2bitreader_read(&br, 32);
	mu_check(d == 0x00104D4A);
	br.cursor = 0; br.bitsRead = 0; br.bitCursor = 0;
	d = d2bitreader_read(&br, 63);
	mu_check(d == 0x006500A000104D4A);
}

MU_TEST(item_code_raw)
{
	char c = (char)d2bitreader_read_raw(simpleItemData, 76, 8);
	mu_check(c == 'r');
	c = (char)d2bitreader_read_raw(simpleItemData, 84, 8);
	mu_check(c == '1');
	c = (char)d2bitreader_read_raw(simpleItemData, 92, 8);
	mu_check(c == '1');
	c = (char)d2bitreader_read_raw(simpleItemData, 100, 8);
	mu_check(c == ' ');
}

MU_TEST(simple_flag)
{
	d2bitreader br = { advancedItemData, advancedItemDataSize };
	d2bitreader_skip(&br, 37);
	bool isSimple = d2bitreader_read(&br, 1);
	mu_check(!isSimple);
	mu_check(br.bitsRead == 38);

	d2bitreader br2 = { simpleItemData, simpleItemDataSize };
	d2bitreader_skip(&br2, 37);
	isSimple = d2bitreader_read(&br2, 1);
	mu_check(isSimple);
	mu_check(br2.bitsRead == 38);
}

MU_TEST(item_uid)
{
	d2bitreader br = { advancedItemData, advancedItemDataSize };
	d2bitreader_skip(&br, 111);
	uint32_t uid = (uint32_t)d2bitreader_read(&br, 32);
	mu_check(uid == (uint32_t)0x103D7536);
}

MU_TEST(overflow)
{
	d2bitreader br = { simpleItemData, simpleItemDataSize };
	uint64_t j = d2bitreader_read(&br, 8);
	mu_check(j == 'J');
	uint64_t m = d2bitreader_read(&br, 8);
	mu_check(m == 'M');

	d2bitreader_skip(&br, 84);

	uint64_t b = d2bitreader_read(&br, 8);
	mu_check(b == ' ');

	b = d2bitreader_read(&br, 4);
	mu_check(b == 0);

	// we've read to the exact end at this point

	b = d2bitreader_read(&br, 1);
	mu_check(b == 0);
	mu_check(br.cursor == BIT_READER_CURSOR_BEYOND_EOF);

	// any reads from here should be 0
	b = d2bitreader_read(&br, 32);
	mu_check(b == 0);
	mu_check(br.cursor == BIT_READER_CURSOR_BEYOND_EOF);
}

MU_TEST_SUITE(test_d2bitreader)
{
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(item_data);
	MU_RUN_TEST(item_code);
	MU_RUN_TEST(large_read);
	MU_RUN_TEST(item_code_raw);
	MU_RUN_TEST(simple_flag);
	MU_RUN_TEST(item_uid);
	MU_RUN_TEST(overflow);
}

int main()
{
	MU_RUN_SUITE(test_d2bitreader);
	MU_REPORT();
	return MU_EXIT_CODE;
}
