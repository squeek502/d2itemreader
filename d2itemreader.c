#include "d2itemreader.h"
#include "d2txtreader.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

d2data g_d2data = { NULL };

#define BITS_PER_BYTE 8

// Parses the magical property list in the byte queue that belongs to an item
// and returns the list of properties.
void d2itemproplist_parse(bit_reader* br, d2data* data, d2itemproplist* list)
{
	d2itemproplist_init(list);

	while (true)
	{
		uint16_t id = (uint16_t)read_bits(br, 9);

		// If all 9 bits are set, we've hit the end of the list
		if (id == D2DATA_ITEMSTAT_END_ID)
			break;

		d2data_itemstat* stat = &data->itemstats[id];
		d2itemprop prop = { id };

		// this is unrecoverably bad
		assert(stat->saveBits > 0);
		// encode type 4 is only used by stats that were never implemented (time-based stats)
		assert(stat->encode != 4);

		if (stat->encode == 2)
		{
			assert(stat->saveBits == 7 && stat->saveParamBits == 16);
			prop.params[0] = (int)read_bits(br, 6) - stat->saveAdd;
			prop.params[1] = (int)read_bits(br, 10) - stat->saveAdd;
			prop.params[2] = (int)read_bits(br, stat->saveBits) - stat->saveAdd;
			prop.numParams = 3;
		}
		else if (stat->encode == 3)
		{
			assert(stat->saveBits == 16 && stat->saveParamBits == 16);
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
			assert(stat->saveParamBits == 0);
			prop.params[prop.numParams] = (int)read_bits(br, stat->saveBits) - stat->saveAdd;
			prop.numParams++;
		}

		d2itemproplist_append(list, prop);
	}
}

void d2itemproplist_init(d2itemproplist* list)
{
	list->count = 0;
	list->_size = 4;
	list->properties = malloc(list->_size * sizeof(*list->properties));
}

void d2itemproplist_append(d2itemproplist* list, d2itemprop prop)
{
	if (list->count == list->_size)
	{
		list->_size *= 2;
		list->properties = realloc(list->properties, list->_size * sizeof(*list->properties));
	}
	list->properties[list->count++] = prop;
}

void d2itemproplist_destroy(d2itemproplist* list)
{
	free(list->properties);
	list->properties = NULL;
	list->count = list->_size = 0;
}

void d2itemlist_parse(const unsigned char* const data, uint32_t startByte, d2itemlist* items, uint32_t* out_bytesRead)
{
	uint32_t curByte = startByte;
	uint16_t tag = D2ITEMREADER_READ(uint16_t);
	assert(tag == D2_JM_TAG);

	uint16_t numItems = D2ITEMREADER_READ(uint16_t);

	d2itemlist_init(items, numItems);
	d2item* lastSocketedItem = NULL;

	for (uint16_t i = 0; i < numItems; i++)
	{
		uint32_t itemSizeBytes;
		d2item item = { 0 };
		d2item_parse(data, curByte, &item, &itemSizeBytes);
		curByte += itemSizeBytes;

		if (item.locationID == LOCATION_SOCKETED)
		{
			assert(lastSocketedItem != NULL);
			d2itemlist_append(&lastSocketedItem->socketedItems, &item);
		}
		else
		{
			d2itemlist_append(items, &item);

			if (item.numItemsInSockets > 0 && !item.simpleItem)
			{
				lastSocketedItem = &(items->items[items->count - 1]);
				numItems += item.numItemsInSockets;
			}
		}
	}

	*out_bytesRead = curByte - startByte;
}

void d2itemlist_init(d2itemlist* list, size_t initialSize)
{
	list->count = 0;
	list->_size = initialSize;
	list->items = list->_size > 0 ? malloc(list->_size * sizeof(*list->items)) : NULL;
}

void d2itemlist_append(d2itemlist* list, const d2item* const item)
{
	if (list->count == list->_size)
	{
		list->_size = list->_size > 0 ? list->_size * 2 : 4;
		list->items = realloc(list->items, list->_size * sizeof(*list->items));
	}
	list->items[list->count++] = *item;
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

void d2item_parse(const unsigned char* const data, uint32_t startByte, d2item* item, uint32_t* size_out)
{
	uint32_t curByte = startByte;
	uint16_t tag = D2ITEMREADER_READ(uint16_t);
	assert(tag == D2_JM_TAG);

	bit_reader br = { data, curByte, 16 };
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
		for (int i = 0; i < D2_ITEM_CODE_BYTELEN; i++)
		{
			char c = (char)read_bits(&br, 8);
			if (c == ' ') c = '\0';
			item->code[i] = c;
		}
		item->code[D2_ITEM_CODE_STRLEN] = '\0';

		// offset 108
		// If sockets exist, read the items, they'll be 108 bit basic items * nrOfSockets
		item->numItemsInSockets = (uint8_t)read_bits(&br, 3);
		d2itemlist_init(&item->socketedItems, item->numItemsInSockets);
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
		case RARITY_LOW_QUALITY:
			item->lowQualityID = (uint8_t)read_bits(&br, 3);
			break;
		case RARITY_NORMAL:
			// No extra data present
			break;
		case RARITY_HIGH_QUALITY:
			// TODO: Figure out what these 3 bits are on a high quality item
			item->superiorID = (uint8_t)read_bits(&br, 3);
			break;
		case RARITY_MAGIC:
			item->magicPrefix = (uint16_t)read_bits(&br, 11);
			item->magicSuffix = (uint16_t)read_bits(&br, 11);
			break;
		case RARITY_SET:
			item->setID = (uint16_t)read_bits(&br, 12);
			break;
		case RARITY_RARE:
		case RARITY_CRAFTED:
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
		case RARITY_UNIQUE:
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
		if (item->rarity == RARITY_SET)
		{
			setPropertyFlags = (uint8_t)read_bits(&br, 5);
		}

		// MARK: Time to parse 9 bit magical property ids followed by their n bit
		// length values, but only if the item is magical or above.
		d2itemproplist_parse(&br, &g_d2data, &item->magicProperties);

		// Item has more magical property lists due to being a set item
		if (setPropertyFlags)
		{
			for (int i = 0; i < D2_MAX_SET_PROPERTIES; i++)
			{
				unsigned short mask = 1 << i;
				if (setPropertyFlags & mask)
				{
					d2itemproplist_parse(&br, &g_d2data, &item->setBonuses[item->numSetBonuses]);
					item->numSetBonuses++;
				}
			}
		}

		if (item->isRuneword)
		{
			d2itemproplist_parse(&br, &g_d2data, &item->runewordProperties);
		}
	}

	uint32_t bytesRead = (uint32_t)(br.bitsRead / BITS_PER_BYTE + (br.bitsRead % BITS_PER_BYTE != 0));
	curByte = startByte + bytesRead;

	*size_out = curByte - startByte;
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

void d2stashpage_parse(const unsigned char* const data, uint32_t startByte, d2stashpage *page, uint32_t* out_bytesRead)
{
	uint32_t curByte = startByte;
	uint16_t tag = D2ITEMREADER_READ(uint16_t);
	assert(tag == PLUGY_STASH_TAG);

	page->flags = 0;
	size_t len = strlen((char*)&data[curByte]);
	if (*(uint16_t*)&data[curByte + len + 1] != D2_JM_TAG)
	{
		page->flags = D2ITEMREADER_READ(uint32_t);
	}

	page->name[0] = 0;
	char* namePtr = (char*)&data[curByte];
	size_t nameLen = strlen(namePtr);
	if (nameLen)
		strncpy_s(page->name, sizeof(page->name), namePtr, nameLen + 1);
	curByte += (uint32_t)(nameLen + 1);

	uint32_t inventorySizeBytes;
	d2itemlist_parse(data, curByte, &page->items, &inventorySizeBytes);

	*out_bytesRead = curByte - startByte + inventorySizeBytes;
}

void d2stashpage_destroy(d2stashpage *page)
{
	d2itemlist_destroy(&page->items);
}

void d2sharedstash_parse(const char* filename, d2sharedstash *stash, uint32_t* out_bytesRead)
{
	unsigned char* data;
	size_t size;
	read_full_file(filename, &data, &size);

	uint32_t curByte = 0;

	uint32_t header = D2ITEMREADER_READ(uint32_t);
	assert(header == PLUGY_SHAREDSTASH_HEADER);

	stash->fileVersion = D2ITEMREADER_READ(uint16_t);
	assert(stash->fileVersion == PLUGY_FILE_VERSION_01 || stash->fileVersion == PLUGY_FILE_VERSION_02);

	stash->sharedGold = 0;
	if (stash->fileVersion == PLUGY_FILE_VERSION_02)
	{
		stash->sharedGold = D2ITEMREADER_READ(uint32_t);
	}

	stash->numPages = D2ITEMREADER_READ(uint32_t);
	stash->pages = stash->numPages > 0 ? malloc(stash->numPages * sizeof(*stash->pages)) : NULL;

	int pageNum = 0;
	uint32_t stashSizeBytes;
	while (curByte < size)
	{
		d2stashpage_parse(data, curByte, &stash->pages[pageNum], &stashSizeBytes);
		stash->pages[pageNum].pageNum = pageNum + 1;
		curByte += stashSizeBytes;
		pageNum++;
	}

	free(data);
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
	}
}