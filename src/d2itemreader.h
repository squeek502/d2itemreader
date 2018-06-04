#ifndef D2ITEMREADER_H
#define D2ITEMREADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "bitreader.h"
#include "d2data.h"
#include "d2err.h"
#include "d2util.h"

extern d2data g_d2itemreader_data;

// D2
#define D2S_HEADER 0xAA55AA55 // "U.U." where '.' = (unsigned char)170
#define D2S_STATUS_OFFSET 36
#define D2S_STATUS_EXPANSION_MASK (1 << 5)
#define D2S_MERC_ID_OFFSET 179
#define D2S_STATS_OFFSET 765
#define D2S_STATS_HEADER 0x6667 //"gf"
#define D2S_SKILLS_BYTELEN 32 // 2 byte header + 30 bytes
#define D2S_MERC_HEADER 0x666A //"jf"
#define D2S_IRON_GOLEM_HEADER 0x666B //"kf"
#define D2_JM_TAG 0x4D4A //"JM"
#define D2_MAX_CHAR_NAME_STRLEN 15
#define D2_MAX_CHAR_NAME_BYTELEN (D2_MAX_CHAR_NAME_STRLEN+1)
#define D2_MAX_SET_PROPERTIES 5
#define D2_ITEMPROP_MAX_PARAMS 4
#define D2_MAX_RARE_PREFIXES 3
#define D2_MAX_RARE_SUFFIXES 3
#define D2_MAX_RARE_AFFIXES (D2_MAX_RARE_PREFIXES+D2_MAX_RARE_SUFFIXES)

// PlugY
#define D2_MAX_STASH_PAGE_NAME_STRLEN 15
#define D2_MAX_STASH_PAGE_NAME_BYTELEN (D2_MAX_STASH_PAGE_NAME_STRLEN+1)
#define PLUGY_SHAREDSTASH_HEADER 0x00535353 //"SSS\0"
#define PLUGY_PERSONALSTASH_HEADER 0x4D545343 //"CSTM"
#define PLUGY_FILE_VERSION_01 0x3130 //"01"
#define PLUGY_FILE_VERSION_02 0x3230 //"02"
#define PLUGY_STASH_TAG 0x5453 //"ST"

// ATMA/GoMule
#define GOMULE_D2X_FILE_VERSION 96

// TODO: remove this hardcoding, but first need to check
// if the itemtype controls the save format, or if it actually
// is hardcoded to these ids
// Note: It might be controlled by the entries in Books.txt
#define D2ITEMTYPE_TOME_TP "tbk"
#define D2ITEMTYPE_TOME_ID "ibk"

typedef struct d2datafiles {
	const char* armorTxtFilepath;
	const char* weaponsTxtFilepath;
	const char* miscTxtFilepath;
	const char* itemStatCostTxtFilepath;
} d2datafiles;

typedef struct d2databufs {
	char* armorTxt;
	size_t armorTxtSize;
	char* weaponsTxt;
	size_t weaponsTxtSize;
	char* miscTxt;
	size_t miscTxtSize;
	char* itemStatCostTxt;
	size_t itemStatCostTxtSize;
} d2databufs;

/*
* Load the game data needed by d2itemreader. ONE of the following should be called at startup:
* 
* d2itemreader_init_default: Load the default data packaged with d2itemreader
*                            (should work for recent-ish un-modded D2 versions)
* d2itemreader_init_files: Load the data from the file paths given in `files`
* d2itemreader_init_bufs: Load the data from the buffers given in `bufs`
*
* IMPORTANT: d2itemreader_destroy() only needs to be called if the init function returns D2ERR_OK
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2itemreader_init_default();
CHECK_RESULT d2err d2itemreader_init_files(d2datafiles files);
CHECK_RESULT d2err d2itemreader_init_bufs(d2databufs bufs);
/*
* Cleanup memory used by d2itemreader.
*
* IMPORTANT: d2itemreader_destroy() only needs to be called if the init function returns D2ERR_OK
*/
void d2itemreader_destroy();

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
	D2FILETYPE_PLUGY_PERSONAL_STASH,
	D2FILETYPE_ATMA_STASH
};

enum d2filetype d2filetype_get(const unsigned char* data, size_t size);
enum d2filetype d2filetype_of_file(const char* filename);

typedef struct d2item d2item; // forward dec
typedef struct d2itemlist {
	d2item* items;
	size_t count;
	size_t _size;
} d2itemlist;

/*
* Parse the itemlist in `data` starting at `startByte`, and store the result in `items`
*
* Parameters:
*
*   items: A pointer an uninitialized d2itemlist object (i.e. d2itemlist_init has NOT been called on it).
*          If this function returns D2ERR_OK, then `items` will need to be cleaned up with d2itemlist_destroy.
*          If this function returns something other than D2ERR_OK, then items will remain uninitialized.
*
*   out_bytesRead: On D2ERR_OK, set to the total number of bytes used by the item list.
*                  On error, set to the number of bytes successfully parsed before the error.
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2itemlist_parse(const unsigned char* const data, size_t dataSizeBytes, uint32_t startByte, d2itemlist* items, uint32_t* out_bytesRead);
CHECK_RESULT d2err d2itemlist_init(d2itemlist* list, size_t initialSize);
CHECK_RESULT d2err d2itemlist_append(d2itemlist* list, const d2item* const item);
void d2itemlist_destroy(d2itemlist* list);

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

/*
* Parse the item property list using `br`, and store the result in `list`
*
* Parameters:
*
*   list: A pointer an uninitialized d2itemproplist object (i.e. d2itemproplist_init has NOT been called on it).
*         If this function returns D2ERR_OK, then `list` will need to be cleaned up with d2itemproplist_destroy.
*         If this function returns something other than D2ERR_OK, then `list` will remain uninitialized.
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2itemproplist_parse(bit_reader* br, d2data* data, d2itemproplist* list);
CHECK_RESULT d2err d2itemproplist_init(d2itemproplist* list);
CHECK_RESULT d2err d2itemproplist_append(d2itemproplist* list, d2itemprop prop);
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
	// is ID 0, and only the "Expansion" row is skipped when incrementing ID
	uint16_t setID;
	// uniqueID = the row in UniqueItems.txt, where the first non-header row
	// is ID 0, and only the "Expansion" row is skipped when incrementing ID
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

/*
* Parse the item in `data` starting at `startByte`, and store the result in `item`
*
* Parameters:
*
*   item: A pointer to an uninitialized d2item object.
*         If this function returns D2ERR_OK, then `item` will need to be cleaned up with d2item_destroy.
*         If this function returns something other than D2ERR_OK, then `item` will remain uninitialized.
*
*   out_bytesRead: On D2ERR_OK, set to the total number of bytes used by the item.
*                  On error, set to the number of bytes successfully parsed before the error.
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2item_parse(const unsigned char* const data, size_t dataSizeBytes, uint32_t startByte, d2item* item, uint32_t* out_bytesRead);
void d2item_destroy(d2item *item);

typedef struct d2stashpage {
	uint32_t pageNum;
	uint32_t flags;
	char name[D2_MAX_STASH_PAGE_NAME_BYTELEN];
	d2itemlist items;
} d2stashpage;

/*
* Parse the stash page in `data` starting at `startByte`, and store the result in `page`
*
* Parameters:
*
*   page: A pointer to an uninitialized d2stashpage object.
*         If this function returns D2ERR_OK, then `page` will need to be cleaned up with d2stashpage_destroy.
*         If this function returns something other than D2ERR_OK, then `page` will remain uninitialized.
*
*   out_bytesRead: On D2ERR_OK, set to the total number of bytes used by the stash page.
*                  On error, set to the number of bytes successfully parsed before the error.
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2stashpage_parse(const unsigned char* const data, size_t dataSizeBytes, uint32_t startByte, d2stashpage *page, uint32_t* out_bytesRead);
void d2stashpage_destroy(d2stashpage *page);

typedef struct d2sharedstash {
	uint16_t fileVersion;
	uint32_t sharedGold;
	uint32_t numPages;
	d2stashpage* pages;
} d2sharedstash;

/*
* Parse the shared stash in `filename`, and store the result in `stash`
*
* Parameters:
*
*   stash: A pointer to an uninitialized d2sharedstash object.
*          If this function returns D2ERR_OK, then `stash` will need to be cleaned up with d2sharedstash_destroy.
*          If this function returns something other than D2ERR_OK, then `stash` will remain uninitialized.
*
*   out_bytesRead: On D2ERR_OK, set to the total number of bytes in the file.
*                  On error, set to the number of bytes successfully parsed before the error.
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2sharedstash_parse_file(const char* filename, d2sharedstash *stash, uint32_t* out_bytesRead);
CHECK_RESULT d2err d2sharedstash_parse(const unsigned char* const data, size_t dataSizeBytes, d2sharedstash *stash, uint32_t* out_bytesRead);
void d2sharedstash_destroy(d2sharedstash *stash);

typedef struct d2personalstash {
	uint16_t fileVersion;
	uint32_t numPages;
	d2stashpage* pages;
} d2personalstash;

/*
* Parse the personal stash in `filename`, and store the result in `stash`
*
* Parameters:
*
*   stash: A pointer to an uninitialized d2personalstash object.
*          If this function returns D2ERR_OK, then `stash` will need to be cleaned up with d2personalstash_destroy.
*          If this function returns something other than D2ERR_OK, then `stash` will remain uninitialized.
*
*   out_bytesRead: On D2ERR_OK, set to the total number of bytes in the file.
*                  On error, set to the number of bytes successfully parsed before the error.
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2personalstash_parse_file(const char* filename, d2personalstash *stash, uint32_t* out_bytesRead);
CHECK_RESULT d2err d2personalstash_parse(const unsigned char* const data, size_t dataSizeBytes, d2personalstash *stash, uint32_t* out_bytesRead);
void d2personalstash_destroy(d2personalstash *stash);

typedef struct d2char {
	d2itemlist items;
	d2itemlist itemsCorpse;
	d2itemlist itemsMerc;
} d2char;

/*
* Parse the character in `filename`, and store the result in `character`
*
* Parameters:
*
*   character: A pointer to an uninitialized d2char object.
*              If this function returns D2ERR_OK, then `character` will need to be cleaned up with d2char_destroy.
*              If this function returns something other than D2ERR_OK, then `character` will remain uninitialized.
*
*   out_bytesRead: On D2ERR_OK, set to the total number of bytes in the file.
*                  On error, set to the number of bytes successfully parsed before the error.
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2char_parse_file(const char* filename, d2char *character, uint32_t* out_bytesRead);
CHECK_RESULT d2err d2char_parse(const unsigned char* const data, size_t dataSizeBytes, d2char *character, uint32_t* out_bytesRead);
void d2char_destroy(d2char *character);

typedef struct d2atmastash {
	uint16_t fileVersion;
	d2itemlist items;
} d2atmastash;

/*
* Parse the d2x ATMA stash in `filename`, and store the result in `stash`
*
* Parameters:
*
*   stash: A pointer to an uninitialized d2atmastash object.
*          If this function returns D2ERR_OK, then `stash` will need to be cleaned up with d2atmastash_destroy.
*          If this function returns something other than D2ERR_OK, then `stash` will remain uninitialized.
*
*   out_bytesRead: On D2ERR_OK, set to the total number of bytes in the file.
*                  On error, set to the number of bytes successfully parsed before the error.
*
* Return value: D2ERR_OK on success
*/
CHECK_RESULT d2err d2atmastash_parse_file(const char* filename, d2atmastash* stash, uint32_t* out_bytesRead);
CHECK_RESULT d2err d2atmastash_parse(const unsigned char* const data, size_t dataSizeBytes, d2atmastash* stash, uint32_t* out_bytesRead);
void d2atmastash_destroy(d2atmastash* stash);

#ifdef __cplusplus
}
#endif

#endif
