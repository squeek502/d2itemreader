d2itemreader
============

[![Build Status](https://travis-ci.org/squeek502/d2itemreader.svg?branch=master)](https://travis-ci.org/squeek502/d2itemreader)
[![Build status](https://ci.appveyor.com/api/projects/status/8hyrcevhv2wpvg5l/branch/master?svg=true)](https://ci.appveyor.com/project/squeek502/d2itemreader/branch/master)

**work in progress, everything is subject to change**

d2itemreader is a C library for parsing Diablo II character/stash files (`.d2s`, `.d2x`, and `.sss`) and retrieving data about the items contained inside them. It also tries to avoid any assumptions about the game version or game data, so that it can work with modded files (provided the library is initialized with the relevant modded .txt files on startup).

## Usage

Most API functions in d2itemreader.h work in the following way:

- There is a `<struct>_parse` function that takes a pointer to a struct and returns a `d2err` enum.
  + If the function returns `D2ERR_OK`, then the function succeeded and the struct will need to be cleaned up using the corresponding `_destroy` function.
  + If the function returns anything other than `D2ERR_OK`, then the `_destroy` function *does not* need to be called; any allocated memory is cleaned up by the `_parse` or `_init` function before it returns an error.
  + The `out_bytesRead` parameter will always be set regardless of the result of the `_parse` function. On failure, it will contain the number of bytes read before the error occured.

On program startup, you will need to initialize a d2gamedata struct with the data from some of Diablo II's `.txt` files that can be found in its `.mpq` archives. For convenience, `d2itemreader` bundles the relevant data from the latest `.txt` files (1.14d), which can be loaded by calling:

```c
d2gamedata gameData;
d2err err = d2gamedata_init_default(&gameData);
```

If the `d2gamedata_init` function returns `D2ERR_OK`, the following function should be called on shutdown (or when you're done using the d2itemreader library):

```c
d2gamedata_destroy(&gameData);
```

After the `d2gamedata_init` function is called, you can parse files like so:

```c
const char *filename = "path/to/file";

// determine the filetype if it is not known in advance
enum d2filetype filetype = d2filetype_of_file(filename);

if (filetype != D2FILETYPE_D2_CHARACTER)
{
	fprintf(stderr, "File is not a d2 character file: %s\n", filename);
	return;
}

size_t bytesRead;
d2char character;
d2err err = d2char_parse_file(filename, &character, &gameData, &bytesRead);
if (err != D2ERR_OK)
{
	fprintf(stderr, "Failed to parse %s: %s at byte 0x%zx\n", filename, d2err_str(err), bytesRead);
	// don't need to call d2char_destroy, the memory is cleaned up when _parse returns an error
}
else
{
	// do something with the character data
	int numUniques = 0;

	for (int i=0; i<character.items.count; i++)
	{
		d2item* item = &character.items.items[i];
		if (item->rarity == D2RARITY_UNIQUE)
		{
			numUniques++;
		}
	}

	printf("Number of unique items in %s: %d", filename, numUniques);

	// clean up the memory allocated when parsing the character file
	d2char_destroy(&character);
}
```

## Acknowledgements

- [nokka/d2s](https://github.com/nokka/d2s) - much of the d2s parsing of d2itemreader is ported from `nokka/d2s`
