#include "d2itemreader.h"
#include "d2txtreader.h"
#include "minunit.h"

static d2gamedata gameData;
#define CORPUS_PATH_FORMAT "fuzz/corpus/%s"

MU_TEST(check_against_expected_errors)
{
	d2txt_file parsed;
	size_t numRows;
	d2err err = d2txt_parse_file("data/expected_errors.txt", &parsed, &numRows);
	mu_assert_int_eq((int)D2ERR_OK, (int)err);

	int file_path_col = d2txt_find_index(parsed, "file_path");
	mu_check(file_path_col >= 0);
	int error_col = d2txt_find_index(parsed, "error");
	mu_check(error_col >= 0);
	int bytes_read_col = d2txt_find_index(parsed, "bytes_read");
	mu_check(bytes_read_col >= 0);

	for (int iRow = 1; parsed[iRow]; iRow++)
	{
		d2txt_row row = parsed[iRow];
		d2txt_field filePath = row[file_path_col];
		d2txt_field expectedError = row[error_col];
		size_t expectedBytesRead = (size_t)strtol(row[bytes_read_col], NULL, 16);

		static char fullPath[4096];
		sprintf(fullPath, CORPUS_PATH_FORMAT, filePath);

		d2itemlist itemList;
		size_t bytesRead;
		d2err err = d2itemreader_parse_any_file(fullPath, &itemList, &gameData, &bytesRead);
		if (err == D2ERR_OK)
			d2itemlist_destroy(&itemList);

		fprintf(stderr, "%s\n", filePath);
		mu_assert_string_eq(expectedError, d2err_str(err));
		mu_assert_int_eq((int)expectedBytesRead, (int)bytesRead);
	}

	d2txt_destroy(parsed);
}

MU_TEST_SUITE(test_d2itemreader_errors)
{
	d2err err = d2gamedata_init_default(&gameData);
	mu_check(err == D2ERR_OK);
	MU_RUN_TEST(check_against_expected_errors);
	d2gamedata_destroy(&gameData);
}

int main()
{
	MU_RUN_SUITE(test_d2itemreader_errors);
	MU_REPORT();
	return minunit_fail;
}
