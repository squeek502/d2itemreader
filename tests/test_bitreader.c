#include "bitreader.h"
#include "d2util.h"
#include "minunit.h"
#include <stdbool.h>

#define ITEM_SIMPLE_FILE "data/item_simple.bin"
#define ITEM_ADVANCED_FILE "data/item_advanced.bin"

static unsigned char* simpleItemData;
static size_t simpleItemDataSize;
static unsigned char* advancedItemData;
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
	bit_reader br = { simpleItemData, simpleItemDataSize };
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

MU_TEST(item_code)
{
	bit_reader br = { simpleItemData, simpleItemDataSize };
	skip_bits(&br, 76);
	char c = (char)read_bits(&br, 8);
	mu_check(c == 'r');
	c = (char)read_bits(&br, 8);
	mu_check(c == '1');
	c = (char)read_bits(&br, 8);
	mu_check(c == '1');
	c = (char)read_bits(&br, 8);
	mu_check(c == ' ');
}

MU_TEST(large_read)
{
	bit_reader br = { simpleItemData, simpleItemDataSize };
	uint64_t d = read_bits(&br, 1);
	mu_check(d == 0);
	br.cursor = 0; br.bitsRead = 0; br.bitCursor = 0;
	d = read_bits(&br, 2);
	mu_check(d == 2);
	br.cursor = 0; br.bitsRead = 0; br.bitCursor = 0;
	d = read_bits(&br, 32);
	mu_check(d == 0x00104D4A);
	br.cursor = 0; br.bitsRead = 0; br.bitCursor = 0;
	d = read_bits(&br, 63);
	mu_check(d == 0x006500A000104D4A);
}

MU_TEST(item_code_raw)
{
	char c = (char)read_bits_raw(simpleItemData, 76, 8);
	mu_check(c == 'r');
	c = (char)read_bits_raw(simpleItemData, 84, 8);
	mu_check(c == '1');
	c = (char)read_bits_raw(simpleItemData, 92, 8);
	mu_check(c == '1');
	c = (char)read_bits_raw(simpleItemData, 100, 8);
	mu_check(c == ' ');
}

MU_TEST(simple_flag)
{
	bit_reader br = { advancedItemData, advancedItemDataSize };
	skip_bits(&br, 37);
	bool isSimple = read_bits(&br, 1);
	mu_check(!isSimple);
	mu_check(br.bitsRead == 38);

	bit_reader br2 = { simpleItemData, simpleItemDataSize };
	skip_bits(&br2, 37);
	isSimple = read_bits(&br2, 1);
	mu_check(isSimple);
	mu_check(br2.bitsRead == 38);
}

MU_TEST(item_uid)
{
	bit_reader br = { advancedItemData, advancedItemDataSize };
	skip_bits(&br, 111);
	uint32_t uid = (uint32_t)read_bits(&br, 32);
	mu_check(uid == (uint32_t)0x103D7536);
}

MU_TEST(overflow)
{
	bit_reader br = { simpleItemData, simpleItemDataSize };
	uint64_t j = read_bits(&br, 8);
	mu_check(j == 'J');
	uint64_t m = read_bits(&br, 8);
	mu_check(m == 'M');

	skip_bits(&br, 84);

	uint64_t b = read_bits(&br, 8);
	mu_check(b == ' ');

	b = read_bits(&br, 4);
	mu_check(b == 0);

	// we've read to the exact end at this point

	b = read_bits(&br, 1);
	mu_check(b == 0);
	mu_check(br.cursor == BIT_READER_CURSOR_BEYOND_EOF);

	// any reads from here should be 0
	b = read_bits(&br, 32);
	mu_check(b == 0);
	mu_check(br.cursor == BIT_READER_CURSOR_BEYOND_EOF);
}

MU_TEST_SUITE(test_bitreader)
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
	MU_RUN_SUITE(test_bitreader);
	MU_REPORT();
	return minunit_status;
}
