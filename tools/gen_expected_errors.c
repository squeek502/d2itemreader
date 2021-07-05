///
/// Generates a .txt file from a corpus of input files
/// The format will be a tab-separted csv file that is readable by d2txtreader
/// where the headers are file_path, error, and bytes_read
///
/// The idea here is to be able to compare the error outputs between
/// revisions to detect regressions or unintended functionality changes.
///

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <linux/limits.h>
#define MAX_PATH PATH_MAX
#endif

#include "d2itemreader.h"

static d2gamedata gameData;
static const char* corpus_dir_path;

d2err testFile(const char* filename, size_t* out_bytesRead)
{
	d2itemlist itemList;
	d2err err = d2itemreader_parse_any_file(filename, &itemList, &gameData, out_bytesRead);
	if (err == D2ERR_OK)
		d2itemlist_destroy(&itemList);
	return err;
}

void reportError(const char* path)
{
	size_t bytesRead;
	d2err err = testFile(path, &bytesRead);

	// make path relative to input dir and normalize path separators
	static char relativePath[MAX_PATH];
	const char* relativePathStart = path + strlen(corpus_dir_path);
	while (relativePathStart[0] == '/' || relativePathStart[0] == '\\')
		relativePathStart++;
	// copy but replace \ with /
	size_t i = 0;
	for (; i<strlen(relativePathStart); i++)
	{
		char c = relativePathStart[i];
		if (c == '\\') c = '/';
		relativePath[i] = c;
	}
	relativePath[i] = '\0';

	printf("%s\t%s\t%zx\n", relativePath, d2err_str(err), bytesRead);
}

void forEachFile(const char* startDir, void (*callback)());

int main(int argc, const char* argv[])
{
	if (argc <= 1)
	{
		fprintf(stderr, "usage: %s input_dir\n", argv[0]);
		return 1;
	}

	corpus_dir_path = argv[1];

	d2err err = d2gamedata_init_default(&gameData);
	if (err != D2ERR_OK)
	{
		fprintf(stderr, "Failed to initialize default data: %s\n", d2err_str(err));
		return 1;
	}

	printf("file_path\terror\tbytes_read\n");
	forEachFile(corpus_dir_path, &reportError);

	d2gamedata_destroy(&gameData);
	return 0;
}

void forEachFile(const char* startDir, void (*callback)(const char*))
{
#ifdef _WIN32
	char searchPath[MAX_PATH];
	sprintf(searchPath, "%s\\*", startDir);
	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(searchPath, &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		char err[256];
		int strLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			GetSystemDefaultLangID(), // Default language
			err, 256, NULL
		);
		err[strLen - 2] = '\0'; // strip the \r\n
		fprintf(stderr, "Error opening dir: %s: %s\n", err, startDir);
		return;
	}

	do
	{
		if (strcmp(".", FindData.cFileName) == 0 || strcmp("..", FindData.cFileName) == 0)
			continue;

		char *path = (char*)malloc(MAX_PATH);
		sprintf(path, "%s\\%s", startDir, FindData.cFileName);

		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			forEachFile(path, callback);
		}
		else
		{
			callback(path);
		}

		free(path);
	}
	while (FindNextFile(hFind, &FindData));

	FindClose(hFind);
#else
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(startDir)) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
				continue;

			char *path = (char*)malloc(MAX_PATH);
			sprintf(path, "%s/%s", startDir, ent->d_name);

			if (ent->d_type == DT_DIR)
			{
				forEachFile(path, callback);
			}
			else
			{
				callback(path);
			}

			free(path);
		}
		closedir(dir);
	}
	else
	{
		fprintf(stderr, "Error opening dir: %s: %s\n", strerror(errno), startDir);
	}
#endif
}
