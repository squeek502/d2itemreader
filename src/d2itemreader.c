#include "d2itemreader.h"
#include "d2txtreader.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

d2data g_d2data = { NULL };

#define BITS_PER_BYTE 8
#define D2ITEMREADER_DATA (data + curByte)
#define D2ITEMREADER_INC(T) curByte += sizeof(T)
#define D2ITEMREADER_SKIP(T) if (curByte+sizeof(T)<=dataSizeBytes) { D2ITEMREADER_INC(T); }
#define D2ITEMREADER_READ(T) (curByte+sizeof(T)<=dataSizeBytes ? *(T*)D2ITEMREADER_DATA : (T)0); D2ITEMREADER_SKIP(T)

enum d2filetype d2filetype_get(const unsigned char* data, size_t size)
{
	if (size < 4)
		return D2FILETYPE_UNKNOWN;

	if (data[0] == 'D' && data[1] == '2' && data[2] == 'X')
	{
		return D2FILETYPE_ATMA_STASH;
	}

	uint32_t header = *(uint32_t*)(data);
	switch (header)
	{
	case D2S_HEADER:
		return D2FILETYPE_D2_CHARACTER;
	case PLUGY_PERSONALSTASH_HEADER:
		return D2FILETYPE_PLUGY_PERSONAL_STASH;
	case PLUGY_SHAREDSTASH_HEADER:
		return D2FILETYPE_PLUGY_SHARED_STASH;
	default:
		return D2FILETYPE_UNKNOWN;
	}
}

enum d2filetype d2filetype_of_file(const char* filename)
{
	FILE* file = fopen(filename, "rb");

	if (file == NULL)
		return D2FILETYPE_UNKNOWN;

	uint32_t header;
	size_t bytesRead = fread(&header, 1, 4, file);
	fclose(file);

	return d2filetype_get((unsigned char*)&header, bytesRead);
}

// Parses the magical property list in the byte queue that belongs to an item
// and returns the list of properties.
CHECK_RESULT d2err d2itemproplist_parse(bit_reader* br, d2data* data, d2itemproplist* list)
{
	d2err err;
	if ((err = d2itemproplist_init(list)) != D2ERR_OK)
	{
		return err;
	}

	while (true)
	{
		uint16_t id = (uint16_t)read_bits(br, 9);

		if (br->cursor == BIT_READER_CURSOR_BEYOND_EOF)
		{
			err = D2ERR_PARSE_UNEXPECTED_EOF;
			goto err;
		}

		// If all 9 bits are set, we've hit the end of the list
		if (id == D2DATA_ITEMSTAT_END_ID)
			break;

		if (id > D2DATA_ITEMSTAT_END_ID)
		{
			err = D2ERR_PARSE;
			goto err;
		}

		d2data_itemstat* stat = &data->itemstats[id];
		d2itemprop prop = { id };

		// saveBits being zero is unrecoverably bad, and
		// encode type 4 is only used by stats that were never implemented (time-based stats)
		if (stat->saveBits == 0 || stat->encode == 4)
		{
			err = D2ERR_PARSE;
			goto err;
		}

		if (stat->encode == 2)
		{
			if (!(stat->saveBits == 7 && stat->saveParamBits == 16))
			{
				err = D2ERR_PARSE;
				goto err;
			}
			prop.params[0] = (int)read_bits(br, 6) - stat->saveAdd;
			prop.params[1] = (int)read_bits(br, 10) - stat->saveAdd;
			prop.params[2] = (int)read_bits(br, stat->saveBits) - stat->saveAdd;
			prop.numParams = 3;
		}
		else if (stat->encode == 3)
		{
			if (!(stat->saveBits == 16 && stat->saveParamBits == 16))
			{
				err = D2ERR_PARSE;
				goto err;
			}
			prop.params[0] = (int)read_bits(br, 6) - stat->saveAdd;
			prop.params[1] = (int)read_bits(br, 10) - stat->saveAdd;
			prop.params[2] = (int)read_bits(br, 8) - stat->saveAdd;
			prop.params[3] = (int)read_bits(br, 8) - stat->saveAdd;
			prop.numParams = 4;
		}
		else if (stat->saveParamBits > 0)
		{
			prop.params[0] = (int)read_bits(br, stat->saveParamBits) - stat->saveAdd;
			prop.params[1] = (int)read_bits(br, stat->saveBits) - stat->saveAdd;
			prop.numParams = 2;
		}
		else
		{
			prop.params[0] = (int)read_bits(br, stat->saveBits) - stat->saveAdd;
			prop.numParams = 1;
		}

		while (stat->nextInChain && prop.numParams < D2_ITEMPROP_MAX_PARAMS)
		{
			stat = &data->itemstats[stat->nextInChain];
			if (stat->saveParamBits != 0)
			{
				err = D2ERR_PARSE;
				goto err;
			}
			prop.params[prop.numParams] = (int)read_bits(br, stat->saveBits) - stat->saveAdd;
			prop.numParams++;
		}

		if ((err = d2itemproplist_append(list, prop)) != D2ERR_OK)
		{
			goto err;
		}
	}
	return D2ERR_OK;

err:
	d2itemproplist_destroy(list);
	return err;
}

CHECK_RESULT d2err d2itemproplist_init(d2itemproplist* list)
{
	list->count = 0;
	list->_size = 4;
	list->properties = malloc(list->_size * sizeof(*list->properties));
	return list->properties != NULL ? D2ERR_OK : D2ERR_OUT_OF_MEMORY;
}

CHECK_RESULT d2err d2itemproplist_append(d2itemproplist* list, d2itemprop prop)
{
	if (list->count == list->_size)
	{
		list->_size *= 2;
		void* tmp = realloc(list->properties, list->_size * sizeof(*list->properties));
		if (tmp == NULL)
		{
			return D2ERR_OUT_OF_MEMORY;
		}
		list->properties = tmp;
	}
	list->properties[list->count++] = prop;
	return D2ERR_OK;
}

void d2itemproplist_destroy(d2itemproplist* list)
{
	if (list->properties)
	{
		free(list->properties);
	}
	list->properties = NULL;
	list->count = list->_size = 0;
}

// Parse the items directly, once the number of items (not including socketed items) is known.
static CHECK_RESULT d2err d2itemlist_parse_items(const unsigned char* const data, size_t dataSizeBytes, uint32_t startByte, d2itemlist* items, uint16_t numItems, uint32_t* out_bytesRead)
{
	d2err err;
	if ((err = d2itemlist_init(items, numItems)) != D2ERR_OK)
	{
		*out_bytesRead = 0;
		return err;
	}

	uint32_t curByte = startByte;
	d2item* lastSocketedItem = NULL;

	for (uint16_t i = 0; i < numItems; i++)
	{
		uint32_t itemSizeBytes;
		d2item item = { 0 };
		if ((err = d2item_parse(data, dataSizeBytes, curByte, &item, &itemSizeBytes)) != D2ERR_OK)
		{
			curByte += itemSizeBytes;
			goto err;
		}

		if (item.locationID == D2LOCATION_SOCKETED)
		{
			if (lastSocketedItem == NULL)
			{
				d2item_destroy(&item);
				err = D2ERR_PARSE_UNEXPECTED_SOCKETED_ITEM;
				goto err;
			}
			if ((err = d2itemlist_append(&lastSocketedItem->socketedItems, &item)) != D2ERR_OK)
			{
				d2item_destroy(&item);
				goto err;
			}
		}
		else
		{
			if (lastSocketedItem != NULL && lastSocketedItem->socketedItems.count != lastSocketedItem->numItemsInSockets)
			{
				d2item_destroy(&item);
				err = D2ERR_PARSE_UNEXPECTED_NONSOCKETED_ITEM;
				goto err;
			}

			if ((err = d2itemlist_append(items, &item)) != D2ERR_OK)
			{
				d2item_destroy(&item);
				goto err;
			}

			if (item.numItemsInSockets > 0 && !item.simpleItem)
			{
				lastSocketedItem = &(items->items[items->count - 1]);
				numItems += item.numItemsInSockets;
			}
			else
			{
				lastSocketedItem = NULL;
			}
		}
		curByte += itemSizeBytes;
	}

	*out_bytesRead = curByte - startByte;
	return D2ERR_OK;

err:
	*out_bytesRead = curByte - startByte;
	d2itemlist_destroy(items);
	return err;
}

CHECK_RESULT d2err d2itemlist_parse(const unsigned char* const data, size_t dataSizeBytes, uint32_t startByte, d2itemlist* items, uint32_t* out_bytesRead)
{
	d2err err;
	uint32_t curByte = startByte;
	uint16_t tag = D2ITEMREADER_READ(uint16_t) else { goto eof; }
	if (tag != D2_JM_TAG)
	{
		*out_bytesRead = 0;
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	uint16_t numItems = D2ITEMREADER_READ(uint16_t) else { goto eof; }

	uint32_t bytesRead;
	err = d2itemlist_parse_items(data, dataSizeBytes, curByte, items, numItems, &bytesRead);
	*out_bytesRead = curByte + bytesRead - startByte;
	return err;

eof:
	*out_bytesRead = (uint32_t)dataSizeBytes - startByte;
	return D2ERR_PARSE_UNEXPECTED_EOF;
}

CHECK_RESULT d2err d2itemlist_init(d2itemlist* list, size_t initialSize)
{
	list->count = 0;
	list->_size = initialSize;
	if (list->_size > 0)
	{
		list->items = malloc(list->_size * sizeof(*list->items));
		if (list->items == NULL)
		{
			return D2ERR_OUT_OF_MEMORY;
		}
	}
	else
	{
		list->items = NULL;
	}
	return D2ERR_OK;
}

CHECK_RESULT d2err d2itemlist_append(d2itemlist* list, const d2item* const item)
{
	if (list->count == list->_size)
	{
		list->_size = list->_size > 0 ? list->_size * 2 : 4;
		void* tmp = realloc(list->items, list->_size * sizeof(*list->items));
		if (tmp == NULL)
		{
			return D2ERR_OUT_OF_MEMORY;
		}
		list->items = tmp;
	}
	list->items[list->count++] = *item;
	return D2ERR_OK;
}

void d2itemlist_destroy(d2itemlist* list)
{
	if (list->items)
	{
		for (unsigned int i = 0; i < list->count; i++)
		{
			d2item_destroy(&(list->items[i]));
		}
		free(list->items);
	}
	list->items = NULL;
	list->count = list->_size = 0;
}

CHECK_RESULT d2err d2item_parse(const unsigned char* const data, size_t dataSizeBytes, uint32_t startByte, d2item* item, uint32_t* out_bytesRead)
{
	d2err err;
	uint32_t curByte = startByte;
	uint16_t tag = D2ITEMREADER_READ(uint16_t) else { goto eof; }
	if (tag != D2_JM_TAG)
	{
		*out_bytesRead = 0;
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	bit_reader br = { data, dataSizeBytes, curByte, 16 };
	// offset: 16, unknown
	skip_bits(&br, 4);
	// offset: 20
	item->identified = read_bits(&br, 1);
	// offset: 21, unknown
	skip_bits(&br, 6);
	// offset: 27
	item->socketed = read_bits(&br, 1);
	// offset 28, unknown
	read_bits(&br, 1);
	// offset 29
	item->isNew = read_bits(&br, 1);
	// offset 30, unknown
	read_bits(&br, 2);
	// offset 32
	item->isEar = read_bits(&br, 1);
	// offset 33
	item->starterItem = read_bits(&br, 1);
	// offset 34, unknown
	read_bits(&br, 3);
	// offset 37, if it's a simple item, it only contains 111 bits data
	item->simpleItem = read_bits(&br, 1);
	// offset 38
	item->ethereal = read_bits(&br, 1);
	// offset 39, unknown
	read_bits(&br, 1);
	// offset 40
	item->personalized = read_bits(&br, 1);
	// offset 41, unknown
	read_bits(&br, 1);
	// offset 42
	item->isRuneword = read_bits(&br, 1);
	// offset 43, unknown
	skip_bits(&br, 5);
	// offset 48, version
	item->version = (uint8_t)read_bits(&br, 8);
	// offset 56, unknown
	skip_bits(&br, 2);
	// offset 58
	item->locationID = (uint8_t)read_bits(&br, 3);
	// offset 61
	item->equippedID = (uint8_t)read_bits(&br, 4);
	// offset 65
	item->positionX = (uint8_t)read_bits(&br, 4);
	// offset 69
	item->positionY = (uint8_t)read_bits(&br, 3);
	// offset 72
	read_bits(&br, 1);
	// offset 73, if item is neither equipped or in the belt, this tells us where it is.
	item->altPositionID = (uint8_t)read_bits(&br, 3);

	if (!item->isEar)
	{
		// offset 76, item type, 4 chars, each 8 bit (not byte aligned)
		// also not null terminated, item codes can be 4 chars long
		for (int i = 0; i < 4; i++)
		{
			char c = (char)read_bits(&br, 8);
			if (c == ' ') c = '\0';
			item->code[i] = c;
		}
		item->code[D2_ITEM_CODE_STRLEN] = '\0';

		// offset 108
		// If sockets exist, read the items, they'll be 108 bit basic items * nrOfSockets
		item->numItemsInSockets = (uint8_t)read_bits(&br, 3);
		if ((err = d2itemlist_init(&item->socketedItems, item->numItemsInSockets)) != D2ERR_OK)
		{
			goto exit;
		}
	}
	else
	{
		// offset 76, the item is an ear, we need to read the ear data.
		item->ear.class = (unsigned int)read_bits(&br, 3);
		item->ear.level = (unsigned int)read_bits(&br, 7);

		for (int i = 0; i < D2_MAX_CHAR_NAME_BYTELEN; i++)
		{
			char c = (char)read_bits(&br, 7);
			item->ear.name[i] = c;
			if (c == '\0') break;
		}
		item->ear.name[D2_MAX_CHAR_NAME_STRLEN] = '\0';
	}

	if (!item->simpleItem)
	{
		// offset 111, item id is 8 chars, each 4 bit
		// note: printf("%08X", item->id) will match what GoMule and
		// Hero Editor display
		item->id = (uint32_t)read_bits(&br, 32);
		// offset 143
		item->level = (uint8_t)read_bits(&br, 7);
		// offset 150
		item->rarity = (uint8_t)read_bits(&br, 4);
		// If this is TRUE, it means the item has more than one picture associated
		// with it.
		item->multiplePictures = read_bits(&br, 1);
		if (item->multiplePictures)
		{
			// The next 3 bits contain the picture ID.
			item->pictureID = (uint8_t)read_bits(&br, 3);
		}

		// If this is TRUE, it means the item is class specific.
		item->classSpecific = read_bits(&br, 1);
		// If the item is class specific, the next 11 bits will
		// contain the class specific data.
		if (item->classSpecific)
		{
			item->automagicID = (uint16_t)read_bits(&br, 11);
		}

		switch (item->rarity)
		{
		case D2RARITY_LOW_QUALITY:
			item->lowQualityID = (uint8_t)read_bits(&br, 3);
			break;
		case D2RARITY_NORMAL:
			// No extra data present
			break;
		case D2RARITY_HIGH_QUALITY:
			// TODO: Figure out what these 3 bits are on a high quality item
			item->superiorID = (uint8_t)read_bits(&br, 3);
			break;
		case D2RARITY_MAGIC:
			item->magicPrefix = (uint16_t)read_bits(&br, 11);
			item->magicSuffix = (uint16_t)read_bits(&br, 11);
			break;
		case D2RARITY_SET:
			item->setID = (uint16_t)read_bits(&br, 12);
			break;
		case D2RARITY_RARE:
		case D2RARITY_CRAFTED:
			item->nameID1 = (uint8_t)read_bits(&br, 8);
			item->nameID2 = (uint8_t)read_bits(&br, 8);

			item->numRarePrefixes = item->numRareSuffixes = 0;
			// Following the name IDs, we got 6 possible magical affixes, the pattern
			// is 1 bit id, 11 bit value... But the value will only exist if the prefix
			// is 1. So we'll read the id first and check it against 1.
			for (int i = 0; i < D2_MAX_RARE_AFFIXES; i++)
			{
				bool hasAffix = read_bits(&br, 1);
				bool isPrefix = i % 2 == 0; // every other affix is a prefix

				if (!hasAffix)
					continue;

				uint16_t affix = (uint16_t)read_bits(&br, 11);

				if (isPrefix)
				{
					item->rarePrefixes[item->numRarePrefixes] = affix;
					item->numRarePrefixes++;
				}
				else
				{
					item->rareSuffixes[item->numRareSuffixes] = affix;
					item->numRareSuffixes++;
				}
			}
			break;
		case D2RARITY_UNIQUE:
			item->uniqueID = (uint16_t)read_bits(&br, 12);
			break;
		}

		if (item->isRuneword)
		{
			item->runewordID = (uint16_t)read_bits(&br, 12);

			// Unknown 4 bits, seems to be 5 all the time.
			skip_bits(&br, 4);
		}

		if (item->personalized)
		{
			for (int i = 0; i < D2_MAX_CHAR_NAME_BYTELEN; i++)
			{
				char c = (char)read_bits(&br, 7);
				item->personalizedName[i] = c;
				if (c == '\0') break;
			}
			item->personalizedName[D2_MAX_CHAR_NAME_STRLEN] = '\0';
		}

		// If the item is a tome, it will contain 5 extra bits, we're not
		// interested in these bits, the value is usually 1, but not sure
		// what it is.
		if (strcmp(item->code, D2ITEMTYPE_TOME_ID) == 0 || strcmp(item->code, D2ITEMTYPE_TOME_TP) == 0)
		{
			read_bits(&br, 5);
		}

		// All items have this field between the personalization (if it exists)
		// and the item specific data
		item->timestamp = read_bits(&br, 1);

		if (d2data_is_armor(item->code, &g_d2data))
		{
			// If the item is an armor, it will have this field of defense data.
			// We need to subtract 10 defense rating from all armors for
			// some reason, I'm not sure why.
			item->defenseRating = (uint16_t)read_bits(&br, 11) - 10;
		}

		if (d2data_is_armor(item->code, &g_d2data) || d2data_is_weapon(item->code, &g_d2data))
		{
			item->maxDurability = (uint8_t)read_bits(&br, 8);
			// Some weapons like phase blades don't have durability, so we'll
			// check if the item has max durability, then we can safely assume
			// it has current durability too.
			if (item->maxDurability > 0)
			{
				item->currentDurability = (uint8_t)read_bits(&br, 8);
				// Seems to be a random bit here.
				read_bits(&br, 1);
			}
		}

		if (d2data_is_stackable(item->code, &g_d2data))
		{
			// If the item is a stacked item, e.g. a javelin or something, these 9
			// bits will contain the quantity.
			item->quantity = (uint16_t)read_bits(&br, 9);
		}

		// If the item is socketed, it will contain 4 bits of data which are the nr
		// of total sockets the item have, regardless of how many are occupied by
		// an item.
		if (item->socketed)
		{
			item->numSockets = (uint8_t)read_bits(&br, 4);
		}

		// If the item is part of a set, these bit will tell us how many lists
		// of magical properties follow the one regular magical property list.
		uint8_t setPropertyFlags = 0;
		if (item->rarity == D2RARITY_SET)
		{
			setPropertyFlags = (uint8_t)read_bits(&br, 5);
		}

		// MARK: Time to parse 9 bit magical property ids followed by their n bit
		// length values, but only if the item is magical or above.
		if ((err = d2itemproplist_parse(&br, &g_d2data, &item->magicProperties)) != D2ERR_OK)
		{
			d2item_destroy(item);
			goto exit;
		}

		// Item has more magical property lists due to being a set item
		if (setPropertyFlags)
		{
			for (int i = 0; i < D2_MAX_SET_PROPERTIES; i++)
			{
				unsigned short mask = 1 << i;
				if (setPropertyFlags & mask)
				{
					if ((err = d2itemproplist_parse(&br, &g_d2data, &item->setBonuses[item->numSetBonuses])) != D2ERR_OK)
					{
						d2item_destroy(item);
						goto exit;
					}
					item->numSetBonuses++;
				}
			}
		}

		if (item->isRuneword)
		{
			if ((err = d2itemproplist_parse(&br, &g_d2data, &item->runewordProperties)) != D2ERR_OK)
			{
				d2item_destroy(item);
				goto exit;
			}
		}
	}

	err = D2ERR_OK;
exit:
	if (br.cursor == BIT_READER_CURSOR_BEYOND_EOF)
	{
		goto eof;
	}
	*out_bytesRead = (uint32_t)(br.bitsRead / BITS_PER_BYTE + (br.bitsRead % BITS_PER_BYTE != 0));
	return err;

eof:
	*out_bytesRead = (uint32_t)dataSizeBytes - startByte;
	return D2ERR_PARSE_UNEXPECTED_EOF;
}

void d2item_destroy(d2item *item)
{
	d2itemlist_destroy(&item->socketedItems);

	d2itemproplist_destroy(&item->magicProperties);
	d2itemproplist_destroy(&item->runewordProperties);
	for (int i = 0; i < item->numSetBonuses; i++)
	{
		d2itemproplist_destroy(&item->setBonuses[i]);
	}
}

CHECK_RESULT d2err d2stashpage_parse(const unsigned char* const data, size_t dataSizeBytes, uint32_t startByte, d2stashpage *page, uint32_t* out_bytesRead)
{
	d2err err;
	uint32_t curByte = startByte;
	uint16_t tag = D2ITEMREADER_READ(uint16_t) else { goto eof; }
	if (tag != PLUGY_STASH_TAG)
	{
		*out_bytesRead = 0;
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	page->flags = 0;
	// need to make sure we have at least one byte available to read
	if (curByte >= dataSizeBytes)
	{
		goto eof;
	}
	size_t len = strnlen((char*)&data[curByte], dataSizeBytes - curByte - 1);
	// check that we won't read past the end of data here
	if (curByte + len + 1 + sizeof(uint16_t) > dataSizeBytes)
	{
		goto eof;
	}
	if (*(uint16_t*)&data[curByte + len + 1] != D2_JM_TAG)
	{
		page->flags = D2ITEMREADER_READ(uint32_t) else { goto eof; }
	}

	// need to make sure we have at least one byte available to read
	if (curByte >= dataSizeBytes)
	{
		goto eof;
	}
	page->name[0] = 0;
	char* namePtr = (char*)&data[curByte];
	size_t nameLen = strnlen(namePtr, dataSizeBytes - curByte - 1);
	if (nameLen > D2_MAX_STASH_PAGE_NAME_STRLEN)
	{
		*out_bytesRead = curByte;
		return D2ERR_PARSE_STRING_TOO_LONG;
	}
	if (nameLen)
		strncpy(page->name, namePtr, nameLen + 1);
	curByte += (uint32_t)(nameLen + 1);

	uint32_t inventorySizeBytes;
	if ((err = d2itemlist_parse(data, dataSizeBytes, curByte, &page->items, &inventorySizeBytes)) != D2ERR_OK)
	{
		goto exit;
	}

	err = D2ERR_OK;
exit:
	*out_bytesRead = curByte - startByte + inventorySizeBytes;
	return err;

eof:
	*out_bytesRead = (uint32_t)dataSizeBytes - startByte;
	return D2ERR_PARSE_UNEXPECTED_EOF;
}

void d2stashpage_destroy(d2stashpage *page)
{
	d2itemlist_destroy(&page->items);
}

CHECK_RESULT d2err d2sharedstash_parse_file(const char* filename, d2sharedstash *stash, uint32_t* out_bytesRead)
{
	d2err err;
	unsigned char* data;
	size_t dataSizeBytes;
	err = d2util_read_file(filename, &data, &dataSizeBytes);
	if (err != D2ERR_OK)
	{
		*out_bytesRead = 0;
		return err;
	}
	err = d2sharedstash_parse(data, dataSizeBytes, stash, out_bytesRead);
	free(data);
	return err;
}

CHECK_RESULT d2err d2sharedstash_parse(const unsigned char* const data, size_t dataSizeBytes, d2sharedstash *stash, uint32_t* out_bytesRead)
{
	d2err err;
	uint32_t curByte = 0;

	uint32_t header = D2ITEMREADER_READ(uint32_t) else { goto eof; }
	if (header != PLUGY_SHAREDSTASH_HEADER)
	{
		*out_bytesRead = 0;
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	stash->fileVersion = D2ITEMREADER_READ(uint16_t) else { goto eof; }
	if (!(stash->fileVersion == PLUGY_FILE_VERSION_01 || stash->fileVersion == PLUGY_FILE_VERSION_02))
	{
		*out_bytesRead = curByte - sizeof(uint16_t);
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	stash->sharedGold = 0;
	if (stash->fileVersion == PLUGY_FILE_VERSION_02)
	{
		stash->sharedGold = D2ITEMREADER_READ(uint32_t) else { goto eof; }
	}
	
	// stash->numPages is incremented as pages are successfully parsed to
	// ensure that calling d2personalstash_destroy doesn't attempt to free pages
	// that haven't been initialized
	stash->numPages = 0;
	uint32_t expectedNumPages = D2ITEMREADER_READ(uint32_t) else { goto eof; }
	// basic sanity check for impossible page numbers
	if (expectedNumPages > dataSizeBytes)
	{
		*out_bytesRead = curByte - sizeof(uint32_t);
		return D2ERR_PARSE_TOO_MANY_STASH_PAGES;
	}
	stash->pages = NULL;
	if (expectedNumPages > 0)
	{
		stash->pages = malloc(expectedNumPages * sizeof(*stash->pages));
		if (stash->pages == NULL)
		{
			*out_bytesRead = curByte;
			return D2ERR_OUT_OF_MEMORY;
		}
	}

	int pageNum = 0;
	uint32_t stashSizeBytes;
	while (stash->numPages < expectedNumPages && curByte < dataSizeBytes)
	{
		if ((err = d2stashpage_parse(data, dataSizeBytes, curByte, &stash->pages[stash->numPages], &stashSizeBytes)) != D2ERR_OK)
		{
			*out_bytesRead = curByte + stashSizeBytes;
			d2sharedstash_destroy(stash);
			return err;
		}
		stash->pages[stash->numPages].pageNum = stash->numPages + 1;
		curByte += stashSizeBytes;
		stash->numPages++;
	}

	*out_bytesRead = curByte;
	if (stash->numPages != expectedNumPages)
	{
		d2sharedstash_destroy(stash);
		return D2ERR_PARSE_TOO_FEW_STASH_PAGES;
	}
	if (curByte != dataSizeBytes)
	{
		d2sharedstash_destroy(stash);
		return D2ERR_PARSE_TRAILING_BYTES;
	}
	return D2ERR_OK;

eof:
	*out_bytesRead = (uint32_t)dataSizeBytes;
	return D2ERR_PARSE_UNEXPECTED_EOF;
}

void d2sharedstash_destroy(d2sharedstash *stash)
{
	if (stash->pages)
	{
		for (unsigned int i = 0; i < stash->numPages; i++)
		{
			d2stashpage_destroy(&stash->pages[i]);
		}
		free(stash->pages);
		stash->pages = NULL;
	}
}

CHECK_RESULT d2err d2personalstash_parse_file(const char* filename, d2personalstash *stash, uint32_t* out_bytesRead)
{
	d2err err;
	unsigned char* data;
	size_t dataSizeBytes;
	err = d2util_read_file(filename, &data, &dataSizeBytes);
	if (err != D2ERR_OK)
	{
		*out_bytesRead = 0;
		return err;
	}
	err = d2personalstash_parse(data, dataSizeBytes, stash, out_bytesRead);
	free(data);
	return err;
}

CHECK_RESULT d2err d2personalstash_parse(const unsigned char* const data, size_t dataSizeBytes, d2personalstash *stash, uint32_t* out_bytesRead)
{
	d2err err;
	uint32_t curByte = 0;

	uint32_t header = D2ITEMREADER_READ(uint32_t) else { goto eof; }
	if (header != PLUGY_PERSONALSTASH_HEADER)
	{
		*out_bytesRead = 0;
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	stash->fileVersion = D2ITEMREADER_READ(uint16_t) else { goto eof; }
	if (!(stash->fileVersion == PLUGY_FILE_VERSION_01))
	{
		*out_bytesRead = curByte - sizeof(uint16_t);
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	// unused block
	D2ITEMREADER_SKIP(uint32_t) else { goto eof; }

	// stash->numPages is incremented as pages are successfully parsed to
	// ensure that calling d2personalstash_destroy doesn't attempt to free pages
	// that haven't been initialized
	stash->numPages = 0;
	uint32_t expectedNumPages = D2ITEMREADER_READ(uint32_t) else { goto eof; }
	// basic sanity check for impossible page numbers
	if (expectedNumPages > dataSizeBytes)
	{
		*out_bytesRead = curByte - sizeof(uint32_t);
		return D2ERR_PARSE_TOO_MANY_STASH_PAGES;
	}
	stash->pages = NULL;
	if (expectedNumPages > 0)
	{
		stash->pages = malloc(expectedNumPages * sizeof(*stash->pages));
		if (stash->pages == NULL)
		{
			*out_bytesRead = curByte;
			return D2ERR_OUT_OF_MEMORY;
		}
	}

	uint32_t stashSizeBytes;
	while (stash->numPages < expectedNumPages && curByte < dataSizeBytes)
	{
		if ((err = d2stashpage_parse(data, dataSizeBytes, curByte, &stash->pages[stash->numPages], &stashSizeBytes)) != D2ERR_OK)
		{
			*out_bytesRead = curByte + stashSizeBytes;
			d2personalstash_destroy(stash);
			return err;
		}
		stash->pages[stash->numPages].pageNum = stash->numPages + 1;
		curByte += stashSizeBytes;
		stash->numPages++;
	}

	*out_bytesRead = curByte;
	if (stash->numPages != expectedNumPages)
	{
		d2personalstash_destroy(stash);
		return D2ERR_PARSE_TOO_FEW_STASH_PAGES;
	}
	if (curByte != dataSizeBytes)
	{
		d2personalstash_destroy(stash);
		return D2ERR_PARSE_TRAILING_BYTES;
	}
	return D2ERR_OK;

eof:
	*out_bytesRead = (uint32_t)dataSizeBytes;
	return D2ERR_PARSE_UNEXPECTED_EOF;
}

void d2personalstash_destroy(d2personalstash *stash)
{
	if (stash->pages)
	{
		for (unsigned int i = 0; i < stash->numPages; i++)
		{
			d2stashpage_destroy(&stash->pages[i]);
		}
		free(stash->pages);
	}
}

CHECK_RESULT d2err d2char_parse_file(const char* filename, d2char *character, uint32_t* out_bytesRead)
{
	d2err err;
	unsigned char* data;
	size_t dataSizeBytes;
	err = d2util_read_file(filename, &data, &dataSizeBytes);
	if (err != D2ERR_OK)
	{
		*out_bytesRead = 0;
		return err;
	}
	err = d2char_parse(data, dataSizeBytes, character, out_bytesRead);
	free(data);
	return err;
}

CHECK_RESULT d2err d2char_parse(const unsigned char* const data, size_t dataSizeBytes, d2char *character, uint32_t* out_bytesRead)
{
	d2err err;
	uint32_t curByte = 0;

	if (dataSizeBytes < D2S_STATS_OFFSET)
	{
		*out_bytesRead = (uint32_t)dataSizeBytes;
		return D2ERR_PARSE_NOT_ENOUGH_BYTES;
	}

	uint8_t statusBitfield = *(uint8_t*)(data + D2S_STATUS_OFFSET);
	bool isExpansion = statusBitfield & D2S_STATUS_EXPANSION_MASK;
	uint32_t mercID = isExpansion ? *(uint32_t*)(data + D2S_MERC_ID_OFFSET) : 0;

	// skip to stats, as that's where things gets variable length
	curByte = D2S_STATS_OFFSET;

	uint16_t header = D2ITEMREADER_READ(uint16_t) else { goto eof; }
	if (header != D2S_STATS_HEADER)
	{
		curByte -= sizeof(uint16_t);
		err = D2ERR_PARSE_BAD_HEADER_OR_TAG;
		goto err;
	}

	bit_reader br = { data, dataSizeBytes, curByte };

	while (true)
	{
		uint16_t id = (uint16_t)read_bits(&br, 9);

		if (br.cursor == BIT_READER_CURSOR_BEYOND_EOF)
		{
			goto eof;
		}

		if (id == D2DATA_ITEMSTAT_END_ID)
			break;

		if (id > D2DATA_ITEMSTAT_END_ID)
		{
			curByte = (uint32_t)br.cursor;
			err = D2ERR_PARSE;
			goto err;
		}

		d2data_itemstat* stat = &g_d2data.itemstats[id];

		// this is unrecoverably bad
		if (stat->charSaveBits == 0)
		{
			curByte = (uint32_t)br.cursor;
			err = D2ERR_PARSE;
			goto err;
		}

		skip_bits(&br, stat->charSaveBits);
	}

	curByte = (uint32_t)(br.cursor + D2S_SKILLS_BYTELEN);

	if (curByte > dataSizeBytes || br.cursor == BIT_READER_CURSOR_BEYOND_EOF)
	{
		goto eof;
	}

	uint32_t bytesRead;
	if ((err = d2itemlist_parse(data, dataSizeBytes, curByte, &character->items, &bytesRead)) != D2ERR_OK)
	{
		curByte += bytesRead;
		goto err;
	}

	curByte += bytesRead;

	uint16_t corpseHeader = D2ITEMREADER_READ(uint16_t) else { goto eof_after_items; }
	if (corpseHeader != D2_JM_TAG)
	{
		curByte -= sizeof(uint16_t);
		err = D2ERR_PARSE_BAD_HEADER_OR_TAG;
		goto err_after_items;
	}

	uint16_t isDead = D2ITEMREADER_READ(uint16_t) else { goto eof_after_items; }
	if (isDead)
	{
		// 12 unknown bytes
		curByte += 12;
		// itemlist
		err = d2itemlist_parse(data, dataSizeBytes, curByte, &character->itemsCorpse, &bytesRead);
		curByte += bytesRead;
	}
	else
		err = d2itemlist_init(&character->itemsCorpse, 0);

	if (err != D2ERR_OK)
	{
		// itemsCorpse is not initialized on error, so just destroy items
		goto err_after_items;
	}

	if (isExpansion)
	{
		uint16_t mercHeader = D2ITEMREADER_READ(uint16_t) else { goto eof_after_corpse; }
		if (mercHeader != D2S_MERC_HEADER)
		{
			err = D2ERR_PARSE_BAD_HEADER_OR_TAG;
			goto err_after_corpse;
		}

		if (mercID)
		{
			err = d2itemlist_parse(data, dataSizeBytes, curByte, &character->itemsMerc, &bytesRead);
			curByte += bytesRead;
		}
		else
			err = d2itemlist_init(&character->itemsMerc, 0);

		if (err != D2ERR_OK)
		{
			// itemsMerc is not initialized on error, so just destroy corpse+items
			goto err_after_corpse;
		}

		uint16_t ironGolemHeader = D2ITEMREADER_READ(uint16_t) else { goto eof_after_merc; }
		if (ironGolemHeader != D2S_IRON_GOLEM_HEADER)
		{
			curByte -= sizeof(uint16_t);
			err = D2ERR_PARSE_BAD_HEADER_OR_TAG;
			goto err_after_merc;
		}

		uint8_t hasIronGolem = D2ITEMREADER_READ(uint8_t) else { goto eof_after_merc; }
		if (hasIronGolem)
		{
			// the iron golem item can have items socketed in it, so we need to parse
			// those as well
			d2itemlist ironGolemItems;
			err = d2itemlist_parse_items(data, dataSizeBytes, curByte, &ironGolemItems, 1, &bytesRead);
			if (err != D2ERR_OK)
			{
				goto err_after_merc;
			}
			d2itemlist_destroy(&ironGolemItems);
			curByte += bytesRead;
		}
	}
	else
	{
		if ((err = d2itemlist_init(&character->itemsMerc, 0)) != D2ERR_OK)
		{
			goto err_after_corpse;
		}
	}

	*out_bytesRead = curByte;
	if (curByte != dataSizeBytes)
	{
		d2char_destroy(character);
		return D2ERR_PARSE_TRAILING_BYTES;
	}
	return D2ERR_OK;

err_after_merc:
	d2itemlist_destroy(&character->itemsMerc);
err_after_corpse:
	d2itemlist_destroy(&character->itemsCorpse);
err_after_items:
	d2itemlist_destroy(&character->items);
err:
	*out_bytesRead = curByte;
	return err;

eof_after_merc:
	d2itemlist_destroy(&character->itemsMerc);
eof_after_corpse:
	d2itemlist_destroy(&character->itemsCorpse);
eof_after_items:
	d2itemlist_destroy(&character->items);
eof:
	*out_bytesRead = (uint32_t)dataSizeBytes;
	return D2ERR_PARSE_UNEXPECTED_EOF;
}

void d2char_destroy(d2char *character)
{
	d2itemlist_destroy(&character->items);
	d2itemlist_destroy(&character->itemsCorpse);
	d2itemlist_destroy(&character->itemsMerc);
}

CHECK_RESULT d2err d2atmastash_parse_file(const char* filename, d2atmastash* stash, uint32_t* out_bytesRead)
{
	d2err err;
	unsigned char* data;
	size_t dataSizeBytes;
	err = d2util_read_file(filename, &data, &dataSizeBytes);
	if (err != D2ERR_OK)
	{
		*out_bytesRead = 0;
		return err;
	}
	err = d2atmastash_parse(data, dataSizeBytes, stash, out_bytesRead);
	free(data);
	return err;
}

CHECK_RESULT d2err d2atmastash_parse(const unsigned char* const data, size_t dataSizeBytes, d2atmastash* stash, uint32_t* out_bytesRead)
{
	d2err err;
	uint32_t curByte = 0;

	char header[3];
	header[0] = D2ITEMREADER_READ(char) else { goto eof; }
	header[1] = D2ITEMREADER_READ(char) else { goto eof; }
	header[2] = D2ITEMREADER_READ(char) else { goto eof; }
	if (!(header[0] == 'D' && header[1] == '2' && header[2] == 'X'))
	{
		*out_bytesRead = 0;
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	uint16_t numItems = D2ITEMREADER_READ(uint16_t) else { goto eof; }
	stash->fileVersion = D2ITEMREADER_READ(uint16_t) else { goto eof; }
	if (stash->fileVersion != GOMULE_D2X_FILE_VERSION)
	{
		*out_bytesRead = curByte - sizeof(uint16_t);
		return D2ERR_PARSE_BAD_HEADER_OR_TAG;
	}

	// 32 bit checksum
	D2ITEMREADER_SKIP(uint32_t) else { goto eof; }

	uint32_t bytesRead;
	err = d2itemlist_parse_items(data, dataSizeBytes, curByte, &stash->items, numItems, &bytesRead);
	curByte += bytesRead;

	*out_bytesRead = curByte;
	if (err == D2ERR_OK && curByte != dataSizeBytes)
	{
		// we only need to destroy here since stash->items only gets initialized on D2ERR_OK
		d2atmastash_destroy(stash);
		err = D2ERR_PARSE_TRAILING_BYTES;
	}
	return err;

eof:
	*out_bytesRead = (uint32_t)dataSizeBytes;
	return D2ERR_PARSE_UNEXPECTED_EOF;
}

void d2atmastash_destroy(d2atmastash* stash)
{
	d2itemlist_destroy(&stash->items);
}
