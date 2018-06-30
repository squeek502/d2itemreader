d2itemreader
============

[![Build Status](https://travis-ci.org/squeek502/d2itemreader.svg?branch=master)](https://travis-ci.org/squeek502/d2itemreader)
[![Build status](https://ci.appveyor.com/api/projects/status/8hyrcevhv2wpvg5l/branch/master?svg=true)](https://ci.appveyor.com/project/squeek502/d2itemreader/branch/master)

**work in progress, everything is subject to change**

d2itemreader is a C library for parsing Diablo II character/stash files (`.d2s`, `.d2x`, and `.sss`) and retrieving data about the items contained inside them. It also tries to avoid any assumptions about the game version or game data, so that it can work with modded files (provided the library is initialized with the relevant modded .txt files on startup).

## Usage

### Overview

Most API functions in d2itemreader.h work in the following way:

- There is a `<struct>_parse` function that takes a pointer to a struct and returns a `d2err` enum.
  + If the function returns `D2ERR_OK`, then the function succeeded and the struct will need to be cleaned up using the corresponding `_destroy` function.
  + If the function returns anything other than `D2ERR_OK`, then the `_destroy` function *does not* need to be called; any allocated memory is cleaned up by the `_parse` or `_init` function before it returns an error.
  + The `out_bytesRead` parameter will always be set regardless of the result of the `_parse` function. On failure, it will contain the number of bytes read before the error occured.

On program startup, the library needs to be initialized with the data from some of Diablo II's `.txt` files that can be found in its `.mpq` archives. For convenience, `d2itemreader` bundles the relevant data from the latest `.txt` files (1.14d), which can be loaded by calling:

```c
d2err err = d2itemreader_init_default();
```

If the `d2itemreader_init` function returns `D2ERR_OK`, the following function should be called on shutdown (or when you're done using the d2itemreader library):

```c
d2itemreader_destroy();
```

After the `d2itemreader_init` function is called, you can parse files like so:

```c
const char *filename = "path/to/file";

// determine the filetype if it is not known in advance
enum d2filetype filetype = d2filetype_of_file(filename);

size_t bytesRead;
d2char character;
d2err err = d2char_parse_file(filename, &character, &bytesRead);
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

### API Reference

#### The `d2item` struct

Field | Description
--- | ---
`bool identified` | 
`bool socketed` | 
`bool isNew` | 
`bool isEar` | 
`bool starterItem` | 
`bool simpleItem` | 
`bool ethereal` | 
`bool personalized` | 
`bool isRuneword` | Diablo II does not save any info that *directly* maps an item to a Runes.txt row. Instead, which runeword the item has is determined by the runes socketed in it, and can be checked against Runes.txt's RuneX columns (in order) to determine which row matches the item's runes. *Note: The game performs this sanity check on every runeword item on load, and removes any that are invalid*
`uint8_t version` | 0 = pre-1.08, 1 = classic, 100 = expansion, 101 = expansion 1.10+
`uint8_t locationID` | see the d2location enum
`uint8_t equippedID` | see the d2equiplocation enum
`uint8_t positionX` | the x coordinate of the item
`uint8_t positionY` | the y coordinate of the item
`uint8_t panelID` | the ID of the page the item is on (main inventory, stash, cube, etc); only set if the item's locationID != D2LOCATION_STORED
`d2ear ear` | only initialized if isEar is true; NOTE: Anything below this will be unitialized when isEar is true
`char code[]` | null-terminated item code, typical string length is 3-4 characters
`uint8_t numItemsInSockets` | 
`d2itemlist socketedItems` | List of items socketed within this item
*NOTE:*` | *All of the following are only set if `simpleItem` is false*
`uint32_t id` | random unique ID assigned to this item; typically displayed using printf("%08X", id)
`uint8_t level` | item level
`uint8_t rarity` | see the d2rarity enum
`bool multiplePictures` | 
`uint8_t pictureID` | 
`bool classSpecific` | 
`uint16_t automagicID` | only set if classSpecific is true. automagicID = the row in automagic.txt, where the first non-header row is ID 0, and no rows are skipped when incrementing ID
`uint8_t lowQualityID` | see d2lowquality enum
`uint8_t superiorID` | related in some way to qualityitems.txt, unsure what the ID <-> row mapping is
`uint16_t magicPrefix` | magicPrefix = the row in MagicPrefix.txt, where the first non-header row is ID 1, and only the "Expansion" row is skipped when incrementing ID (ID 0 is no prefix)
`uint16_t magicSuffix` | magicSuffix = the row in MagicSuffix.txt, where the first non-header row is ID 1, and only the "Expansion" row is skipped when incrementing ID (ID 0 is no suffix)
`uint16_t setID` | setID = the row in SetItems.txt, where the first non-header row is ID 0, and only the "Expansion" row is skipped when incrementing ID
`uint16_t uniqueID` | uniqueID = the row in UniqueItems.txt, where the first non-header row is ID 0, and only the "Expansion" row is skipped when incrementing ID
`uint8_t nameID1` | rare or crafted prefix, where nameID1 = the row in RarePrefix.txt, where the first non-header row is (the max ID in RareSuffix.txt)+1, and no rows are skipped when incrementing ID. For example, with the default txt files: RareSuffix.txt's max ID is 155 ('flange'), therefore, the first non-header row in RarePrefix.txt ('Beast') would be ID 156
`uint8_t nameID2` | nameID2 = the row in RareSuffix.txt, where the first non-header row is ID 1, and no rows are skipped when incrementing ID (ID 0 is no suffix)
`uint16_t rarePrefixes[]` | list of magic prefixes used by this rare/crafted item (see magicPrefix)
`uint8_t numRarePrefixes` | number of valid elements in the rarePrefixes array
`uint16_t rareSuffixes[]` | list of magic suffixes used by this rare/crafted item (see magicSuffix)
`uint8_t numRareSuffixes` | number of valid elements in the rareSuffixes array
`char personalizedName[]` | null-terminated name, not including the 's suffix added by the game
`bool timestamp` | 
`uint16_t defenseRating` | the armor value; only set if the item code is in Armor.txt
`uint8_t maxDurability` | only set if the item code has durability (i.e. is in Armor.txt or Weapons.txt) but can be 0 for items that don't have durability (i.e. phase blade)
`uint8_t currentDurability` | only set if maxDurability > 0
`uint16_t quantity` | only set for stackable items (i.e. the stackable column in its .txt is 1)
`uint8_t numSockets` | number of total sockets in the item (regardless of their filled state)
`d2itemproplist magicProperties` | list of magic properties
`d2itemproplist setBonuses[]` | 
`uint8_t numSetBonuses` | number of valid elements in the setBonuses array
`d2itemproplist runewordProperties` | 


## Acknowledgements

- [nokka/d2s](https://github.com/nokka/d2s) - much of the d2s parsing of d2itemreader is ported from `nokka/d2s`
