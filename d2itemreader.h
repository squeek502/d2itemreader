#ifndef D2ITEMREADER_H
#define D2ITEMREADER_H

#include <stdbool.h>
#include <stdint.h>
#include "bitreader.h"

// D2
#define D2_JM_TAG 0x4D4A //"JM"
#define D2_MAX_CHAR_NAME_STRLEN 15
#define D2_MAX_CHAR_NAME_BYTELEN D2_MAX_CHAR_NAME_STRLEN+1
#define D2_ITEM_CODE_STRLEN 3
#define D2_ITEM_CODE_BYTELEN D2_ITEM_CODE_STRLEN+1
#define D2_MAX_SET_PROPERTIES 5
#define D2_ITEMSTAT_MAX_PARAMS 4
#define D2_ITEMSTAT_END_ID 0x1ff
#define D2_MAX_ITEMSTATCOST_IDS D2_ITEMSTAT_END_ID
#define D2_MAX_RARE_PREFIXES 3
#define D2_MAX_RARE_SUFFIXES 3
#define D2_MAX_RARE_AFFIXES D2_MAX_RARE_PREFIXES+D2_MAX_RARE_SUFFIXES

// PlugY
#define D2_MAX_STASH_PAGE_NAME_STRLEN 15
#define D2_MAX_STASH_PAGE_NAME_BYTELEN D2_MAX_STASH_PAGE_NAME_STRLEN+1
#define PLUGY_SHAREDSTASH_HEADER 0x00535353 //"SSS\0"
#define PLUGY_FILE_VERSION_01 0x3130 //"01"
#define PLUGY_FILE_VERSION_02 0x3230 //"02"
#define PLUGY_STASH_TAG 0x5453 //"ST"

// d2itemreader
#define D2ITEMREADER_DATA (data + curByte)
#define D2ITEMREADER_READ(T) *(T*)D2ITEMREADER_DATA; curByte += sizeof(T)

// TODO: remove this hardcoding
#define D2ITEMTYPE_TOME_TP "tbk"
#define D2ITEMTYPE_TOME_ID "ibk"

typedef struct d2itemreader_data_armor {
	char code[D2_ITEM_CODE_BYTELEN];
} d2itemreader_data_armor;

typedef struct d2itemreader_data_weapon {
	char code[D2_ITEM_CODE_BYTELEN];
	bool stackable;
} d2itemreader_data_weapon;

typedef struct d2itemreader_data_misc {
	char code[D2_ITEM_CODE_BYTELEN];
	bool stackable;
} d2itemreader_data_misc;

typedef struct d2itemreader_data_itemstat {
	uint16_t id;
	uint8_t encode;
	uint16_t saveBits;
	uint16_t saveAdd;
	uint16_t saveParamBits;
	uint16_t nextInChain;
} d2itemreader_data_itemstat;

typedef struct d2itemreader_data {
	d2itemreader_data_armor* armors;
	d2itemreader_data_weapon* weapons;
	d2itemreader_data_misc* miscs;
	d2itemreader_data_itemstat itemstats[D2_MAX_ITEMSTATCOST_IDS];
} d2itemreader_data;

enum d2rarity {
	RARITY_LOW_QUALITY = 0x01,
	RARITY_NORMAL,
	RARITY_HIGH_QUALITY,
	RARITY_MAGIC,
	RARITY_SET,
	RARITY_RARE,
	RARITY_UNIQUE,
	RARITY_CRAFTED
};

enum d2location {
	LOCATION_STORED = 0x00,
	LOCATION_EQUIPPED,
	LOCATION_BELT,
	LOCATION_CURSOR = 0x04,
	LOCATION_SOCKETED = 0x06
};

typedef struct d2ear {
	unsigned int class : 3;
	unsigned int level : 7;
	char name[D2_MAX_CHAR_NAME_BYTELEN];
} d2ear;

typedef struct d2itemprop {
	uint16_t id;
	int params[D2_ITEMSTAT_MAX_PARAMS];
	int numParams;
} d2itemprop;

typedef struct d2itemproplist {
	d2itemprop* properties;
	size_t count;
	size_t _size;
} d2itemproplist;

typedef struct d2itemlist d2itemlist; // forward dec

typedef struct d2item {
	// boolean flags
	bool identified : 1;
	bool socketed : 1;
	bool isNew : 1;
	bool isEar : 1;
	bool starterItem : 1;
	bool simpleItem : 1;
	bool ethereal : 1;
	bool personalized : 1;
	bool isRuneword : 1;
	bool multiplePictures : 1;
	bool classSpecific : 1;
	bool timestamp : 1;
	bool : 4; // force byte alignment
	// ints
	uint32_t id;
	uint8_t locationID;
	uint8_t equippedID;
	uint8_t positionX;
	uint8_t positionY;
	uint8_t altPositionID;
	uint8_t numItemsInSockets;
	uint8_t level;
	uint8_t rarity;
	uint8_t pictureID;
	// the row in automagic.txt
	uint16_t automagicID;
	uint8_t lowQualityID;
	uint8_t superiorID;
	uint16_t magicPrefix;
	uint16_t magicSuffix;
	// setID = the row in SetItems.txt, where the first non-header row
	// is ID 0, and any row that has blank values is skipped when incrementing ID
	// (not sure which col is the one that matters)
	uint16_t setID;
	// uniqueID = the row in UniqueItems.txt, where the first non-header row
	// is ID 0, and any row that has blank values is skipped when incrementing ID
	// (not sure which col is the one that matters)
	uint16_t uniqueID;
	uint16_t defenseRating;
	uint8_t maxDurability;
	uint8_t currentDurability;
	uint8_t numSockets;
	uint8_t nameID1;
	uint8_t nameID2;
	uint16_t quantity;
	uint16_t runewordID;
	// 0 = pre-1.08, 1 = classic, 100 = expansion, 101 = expansion 1.10+ 
	uint8_t version;
	// strings, etc
	char code[D2_ITEM_CODE_BYTELEN];
	char personalizedName[D2_MAX_CHAR_NAME_BYTELEN];
	d2ear ear;
	d2itemproplist magicProperties;
	d2itemproplist setBonuses[D2_MAX_SET_PROPERTIES];
	uint8_t numSetBonuses;
	d2itemproplist runewordProperties;
	uint16_t rarePrefixes[D2_MAX_RARE_PREFIXES];
	uint8_t numRarePrefixes;
	uint16_t rareSuffixes[D2_MAX_RARE_SUFFIXES];
	uint8_t numRareSuffixes;
	d2itemlist* socketedItems;
} d2item;

typedef struct d2itemlist {
	d2item* items;
	size_t count;
	size_t _size;
} d2itemlist;

typedef struct d2stashpage {
	uint32_t pageNum;
	uint32_t flags;
	char name[D2_MAX_STASH_PAGE_NAME_BYTELEN];
	d2itemlist items;
} d2stashpage;

typedef struct d2sharedstash {
	uint16_t fileVersion;
	uint32_t sharedGold;
	uint32_t numPages;
	d2stashpage* pages;
} d2sharedstash;

void load_armors(const char* filename, d2itemreader_data* data);
void load_weapons(const char* filename, d2itemreader_data* data);
void load_miscs(const char* filename, d2itemreader_data* data);
void d2item_parsestats(const char* filename, d2itemreader_data* data);
void d2itemreader_data_destroy(d2itemreader_data* data);

void d2sharedstash_parse(const char* filename, d2sharedstash *stash, uint32_t* out_bytesRead);
void d2sharedstash_destroy(d2sharedstash *stash);

void d2stashpage_parse(const unsigned char* const data, uint32_t startByte, d2stashpage *page, uint32_t* out_bytesRead);
void d2stashpage_destroy(d2stashpage *page);

void d2itemlist_parse(const unsigned char* const data, uint32_t startByte, d2itemlist* items, uint32_t* out_bytesRead);
void d2itemlist_init(d2itemlist* list, size_t initialSize);
void d2itemlist_destroy(d2itemlist* list);
void d2itemlist_append(d2itemlist* list, const d2item* const item);

void d2item_parse(const unsigned char* const data, uint32_t startByte, d2item* item, uint32_t* size_out);
void d2item_destroy(d2item *item);

void d2itemproplist_parse(bit_reader* br, d2itemreader_data* data, d2itemproplist* list);
void d2itemproplist_init(d2itemproplist* list);
void d2itemproplist_append(d2itemproplist* list, d2itemprop prop);
void d2itemproplist_destroy(d2itemproplist* list);

bool is_stackable(const char* itemCode, d2itemreader_data* data);
bool is_weapon(const char* itemCode, d2itemreader_data* data);
bool is_armor(const char* itemCode, d2itemreader_data* data);

extern d2itemreader_data g_d2itemreader_data;

#endif