#ifndef D2ITEMREADER_H
#define D2ITEMREADER_H

#include <stdbool.h>
#include <stdint.h>
#include "bitreader.h"
#include "d2data.h"

extern d2data g_d2data;

// D2
#define D2S_HEADER 0xAA55AA55 // "U.U." where '.' = (unsigned char)170
#define D2S_STATUS_OFFSET 36
#define D2S_STATUS_EXPANSION_MASK 1 << 5
#define D2S_MERC_ID_OFFSET 179
#define D2S_STATS_OFFSET 765
#define D2S_STATS_HEADER 0x6667 //"gf"
#define D2S_SKILLS_BYTELEN 32 // 2 byte header + 30 bytes
#define D2S_MERC_HEADER 0x666A //"jf"
#define D2S_IRON_GOLEM_HEADER 0x666B //"kf"
#define D2_JM_TAG 0x4D4A //"JM"
#define D2_MAX_CHAR_NAME_STRLEN 15
#define D2_MAX_CHAR_NAME_BYTELEN D2_MAX_CHAR_NAME_STRLEN+1
#define D2_MAX_SET_PROPERTIES 5
#define D2_ITEMPROP_MAX_PARAMS 4
#define D2_MAX_RARE_PREFIXES 3
#define D2_MAX_RARE_SUFFIXES 3
#define D2_MAX_RARE_AFFIXES D2_MAX_RARE_PREFIXES+D2_MAX_RARE_SUFFIXES

// PlugY
#define D2_MAX_STASH_PAGE_NAME_STRLEN 15
#define D2_MAX_STASH_PAGE_NAME_BYTELEN D2_MAX_STASH_PAGE_NAME_STRLEN+1
#define PLUGY_SHAREDSTASH_HEADER 0x00535353 //"SSS\0"
#define PLUGY_PERSONALSTASH_HEADER 0x4D545343 //"CSTM"
#define PLUGY_FILE_VERSION_01 0x3130 //"01"
#define PLUGY_FILE_VERSION_02 0x3230 //"02"
#define PLUGY_STASH_TAG 0x5453 //"ST"

// TODO: remove this hardcoding, but first need to check
// if the itemtype controls the save format, or if it actually
// is hardcoded to these ids
#define D2ITEMTYPE_TOME_TP "tbk"
#define D2ITEMTYPE_TOME_ID "ibk"

enum d2rarity {
	D2RARITY_LOW_QUALITY = 0x01,
	D2RARITY_NORMAL,
	D2RARITY_HIGH_QUALITY,
	D2RARITY_MAGIC,
	D2RARITY_SET,
	D2RARITY_RARE,
	D2RARITY_UNIQUE,
	D2RARITY_CRAFTED
};

enum d2location {
	D2LOCATION_STORED = 0x00,
	D2LOCATION_EQUIPPED,
	D2LOCATION_BELT,
	D2LOCATION_CURSOR = 0x04,
	D2LOCATION_SOCKETED = 0x06
};

enum d2filetype {
	D2FILETYPE_UNKNOWN,
	D2FILETYPE_D2_CHARACTER,
	D2FILETYPE_PLUGY_SHARED_STASH,
	D2FILETYPE_PLUGY_PERSONAL_STASH
};

enum d2filetype d2filetype_get(const char* filename);

typedef struct d2item d2item; // forward dec
typedef struct d2itemlist {
	d2item* items;
	size_t count;
	size_t _size;
} d2itemlist;

void d2itemlist_parse(const unsigned char* const data, uint32_t startByte, d2itemlist* items, uint32_t* out_bytesRead);
void d2itemlist_init(d2itemlist* list, size_t initialSize);
void d2itemlist_destroy(d2itemlist* list);
void d2itemlist_append(d2itemlist* list, const d2item* const item);

typedef struct d2itemprop {
	uint16_t id;
	int params[D2_ITEMPROP_MAX_PARAMS];
	int numParams;
} d2itemprop;

typedef struct d2itemproplist {
	d2itemprop* properties;
	size_t count;
	size_t _size;
} d2itemproplist;

void d2itemproplist_parse(bit_reader* br, d2data* data, d2itemproplist* list);
void d2itemproplist_init(d2itemproplist* list);
void d2itemproplist_append(d2itemproplist* list, d2itemprop prop);
void d2itemproplist_destroy(d2itemproplist* list);

typedef struct d2ear {
	unsigned int class : 3;
	unsigned int level : 7;
	char name[D2_MAX_CHAR_NAME_BYTELEN];
} d2ear;

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
	d2itemlist socketedItems;
} d2item;

void d2item_parse(const unsigned char* const data, uint32_t startByte, d2item* item, uint32_t* size_out);
void d2item_destroy(d2item *item);

typedef struct d2stashpage {
	uint32_t pageNum;
	uint32_t flags;
	char name[D2_MAX_STASH_PAGE_NAME_BYTELEN];
	d2itemlist items;
} d2stashpage;

void d2stashpage_parse(const unsigned char* const data, uint32_t startByte, d2stashpage *page, uint32_t* out_bytesRead);
void d2stashpage_destroy(d2stashpage *page);

typedef struct d2sharedstash {
	uint16_t fileVersion;
	uint32_t sharedGold;
	uint32_t numPages;
	d2stashpage* pages;
} d2sharedstash;

void d2sharedstash_parse(const char* filename, d2sharedstash *stash, uint32_t* out_bytesRead);
void d2sharedstash_destroy(d2sharedstash *stash);

typedef struct d2personalstash {
	uint16_t fileVersion;
	uint32_t numPages;
	d2stashpage* pages;
} d2personalstash;

void d2personalstash_parse(const char* filename, d2personalstash *stash, uint32_t* out_bytesRead);
void d2personalstash_destroy(d2personalstash *stash);

typedef struct d2char {
	d2itemlist items;
	d2itemlist itemsCorpse;
	d2itemlist itemsMerc;
} d2char;

void d2char_parse(const char* filename, d2char *character, uint32_t* out_bytesRead);
void d2char_destroy(d2char *character);

#endif