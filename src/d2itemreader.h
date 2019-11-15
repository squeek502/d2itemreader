#ifndef D2ITEMREADER_H
#define D2ITEMREADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "d2err.h"
#include "d2util.h"
#include "d2const.h"
#include "d2gamedata.h"
#include "d2bitreader.h"

// TODO: remove this hardcoding, but first need to check
// if the itemtype controls the save format, or if it actually
// is hardcoded to these ids
// Note: It might be controlled by the entries in Books.txt
#define D2ITEMTYPE_TOME_TP "tbk"
#define D2ITEMTYPE_TOME_ID "ibk"

/// @see d2filetype_get(), d2filetype_of_file()
typedef enum d2filetype
{
	D2FILETYPE_UNKNOWN, ///< Filetype could not be determined or file is malformed
	D2FILETYPE_D2_CHARACTER, ///< Parsable using d2char_parse() or d2char_parse_file()
	D2FILETYPE_PLUGY_SHARED_STASH, ///< Parsable using d2sharedstash_parse() or d2sharedstash_parse_file()
	D2FILETYPE_PLUGY_PERSONAL_STASH, ///< Parsable using d2personalstash_parse() or d2personalstash_parse_file()
	D2FILETYPE_ATMA_STASH, ///< Parsable using d2atmastash_parse() or d2atmastash_parse_file()
	D2FILETYPE_D2_ITEM ///< Parsable using d2item_parse() or d2item_parse_file()
} d2filetype;

/// Attempt to determine the filetype of the given binary data
d2filetype d2filetype_get(const unsigned char* data, size_t size);
/// Attempt to determine the filetype of the given file path
d2filetype d2filetype_of_file(const char* filename);

typedef struct d2item d2item; // forward dec
typedef struct d2itemlist {
	d2item* items;
	size_t count;
	size_t _size;
} d2itemlist;

/**
* Parse the file in `filename` if it is of any parsable format, and store the result in `itemList`
*
* This is a convenience function for when you are only interested in the items in a file, and
* don't care about anything else (i.e. when the metadata you get from more specific parsers is irrelevent)
*
* @param itemList A pointer to an uninitialized d2itemlist object.
*                 If this function returns `D2ERR_OK`, then `itemList` will need to be cleaned up with d2itemlist_destroy().
*                 If this function returns something other than `D2ERR_OK`, then `itemList` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the file.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*/
CHECK_RESULT d2err d2itemreader_parse_any_file(const char* filename, d2itemlist *itemList, d2gamedata *gameData, size_t* out_bytesRead);
CHECK_RESULT d2err d2itemreader_parse_any(const unsigned char* const data, size_t dataSizeBytes, d2itemlist *itemList, d2gamedata *gameData, size_t* out_bytesRead);

/**
* Parse the itemlist in `data` starting at `startByte`, and store the result in `items`
*
* @param items A pointer an uninitialized d2itemlist object (i.e. d2itemlist_init() has NOT been called on it).
*              If this function returns `D2ERR_OK`, then `items` will need to be cleaned up with d2itemlist_destroy().
*              If this function returns something other than `D2ERR_OK`, then `items` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the item list.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*
* @see d2itemlist_destroy(), d2itemlist_parse_num()
*/
CHECK_RESULT d2err d2itemlist_parse(const unsigned char* const data, size_t dataSizeBytes, size_t startByte, d2itemlist* items, d2gamedata *gameData, size_t* out_bytesRead);
/**
* Parse the itemlist containing exactly `numItems` items (not including items in sockets)
* in `data` starting at `startByte`, and store the result in `items`
*
* @param items A pointer an uninitialized d2itemlist object (i.e. d2itemlist_init() has NOT been called on it).
*              If this function returns `D2ERR_OK`, then `items` will need to be cleaned up with d2itemlist_destroy().
*              If this function returns something other than `D2ERR_OK`, then `items` will remain uninitialized.
* @param numItems The number of items to parse, not including items in sockets
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the item list.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*
* @see d2itemlist_destroy(), d2itemlist_parse()
*/
CHECK_RESULT d2err d2itemlist_parse_num(const unsigned char* const data, size_t dataSizeBytes, size_t startByte, d2itemlist* items, uint16_t numItems, d2gamedata *gameData, size_t* out_bytesRead);
CHECK_RESULT d2err d2itemlist_init(d2itemlist* list, size_t initialSize);
void d2itemlist_init_empty(d2itemlist* list);
CHECK_RESULT d2err d2itemlist_append(d2itemlist* list, const d2item* const item);
CHECK_RESULT d2err d2itemlist_append_copy(d2itemlist* list, const d2item* const item);
CHECK_RESULT d2err d2itemlist_append_list_copy(d2itemlist* dest, const d2itemlist* const src);
CHECK_RESULT d2err d2itemlist_copy(d2itemlist* dest, const d2itemlist* const src);
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

/**
* Parse the item property list using `br`, and store the result in `list`
*
* @param br A pointer a d2bitreader struct
* @param list A pointer an uninitialized d2itemproplist object (i.e. d2itemproplist_init() has NOT been called on it).
*             If this function returns `D2ERR_OK`, then `list` will need to be cleaned up with d2itemproplist_destroy().
*             If this function returns something other than `D2ERR_OK`, then `list` will remain uninitialized.
* @return `D2ERR_OK` on success
*
* @see d2itemproplist_destroy()
*/
CHECK_RESULT d2err d2itemproplist_parse(d2bitreader* br, d2itemproplist* list, d2gamedata *gameData);
CHECK_RESULT d2err d2itemproplist_init(d2itemproplist* list, size_t initialSize);
CHECK_RESULT d2err d2itemproplist_append(d2itemproplist* list, d2itemprop prop);
CHECK_RESULT d2err d2itemproplist_copy(d2itemproplist* dest, const d2itemproplist* const src);
void d2itemproplist_destroy(d2itemproplist* list);

enum d2rarity {
	D2RARITY_INVALID,
	D2RARITY_LOW_QUALITY = 0x01,
	D2RARITY_NORMAL,
	D2RARITY_HIGH_QUALITY, ///< AKA Superior
	D2RARITY_MAGIC,
	D2RARITY_SET,
	D2RARITY_RARE,
	D2RARITY_UNIQUE,
	D2RARITY_CRAFTED,
	D2RARITY_TEMPERED, ///< Was never actually enabled in an official release
};

enum d2location {
	D2LOCATION_STORED = 0x00,
	D2LOCATION_EQUIPPED,
	D2LOCATION_BELT,
	D2LOCATION_GROUND, // this should never be in a save file
	D2LOCATION_CURSOR = 0x04,
	// location 5 is unknown
	D2LOCATION_SOCKETED = 0x06
};

enum d2panel {
	D2PANEL_NONE = 0,
	D2PANEL_INVENTORY = 1,
	D2PANEL_CUBE = 4,
	D2PANEL_STASH = 5
};

/// \attention **NOTE**: All equipment locations are based on character orientation
/// on the inventory screen, meaning the right hand is on the left side of the screen
/// (i.e. imagine that the character is oriented as if it is facing towards the screen)
enum d2equiplocation {
	D2EQUIP_HEAD = 1,
	D2EQUIP_NECK = 2,
	D2EQUIP_TORSO = 3,
	D2EQUIP_HAND_RIGHT = 4,
	D2EQUIP_HAND_LEFT = 5,
	D2EQUIP_FINGER_RIGHT = 6,
	D2EQUIP_FINGER_LEFT = 7,
	D2EQUIP_WAIST = 8,
	D2EQUIP_FEET = 9,
	D2EQUIP_HANDS = 10,
	D2EQUIP_ALT_HAND_RIGHT = 11,
	D2EQUIP_ALT_HAND_LEFT = 12,
};

enum d2lowquality {
	D2LOWQUALITY_CRUDE,
	D2LOWQUALITY_CRACKED,
	D2LOWQUALITY_DAMAGED,
	D2LOWQUALITY_LOW_QUALITY,
};

typedef struct d2ear {
	/// class of the player
	uint8_t classID;
	/// level of the player
	uint8_t level;
	/// null-terminated player name
	char name[D2_MAX_CHAR_NAME_BYTELEN];
} d2ear;

struct d2item
{
	bool identified;
	bool socketed;
	bool isNew;
	bool isEar;
	bool starterItem;
	bool simpleItem;
	bool ethereal;
	bool personalized;
	/// Diablo II does not save any info that *directly* maps an item to a Runes.txt row.
	/// Instead, which runeword the item has is determined by the runes socketed in it, and can
	/// be checked against Runes.txt's RuneX columns (in order) to determine which row matches the item's
	/// runes
	///
	/// Note: The game performs this sanity check on every runeword item on load, and removes any that
	/// are invalid
	bool isRuneword;
	/// 0 = pre-1.08, 1 = classic, 100 = expansion, 101 = expansion 1.10+
	uint8_t version;
	/// see the #d2location enum
	uint8_t locationID;
	/// see the #d2equiplocation enum
	uint8_t equippedID;
	/// the x coordinate of the item
	uint8_t positionX;
	/// the y coordinate of the item
	uint8_t positionY;
	/// the ID of the page the item is on (main inventory, stash, cube, etc).
	/// only set if the item's #locationID != `D2LOCATION_STORED`
	uint8_t panelID;

	/// only initialized if isEar is true.
	/// NOTE: Anything below this will be unitialized when isEar is true
	d2ear ear;

	/// null-terminated item code, typical string length is 3-4 characters.
	/// note: space characters are treated as NUL characters when parsing this string
	char code[D2_ITEM_CODE_BYTELEN];
	/// Number of items that are socketed within this item
	uint8_t numItemsInSockets;
	/// List of items socketed within this item
	d2itemlist socketedItems;

	/*
	* NOTE: All of the following are only set if simpleItem is false
	*/

	/// random unique ID assigned to this item
	/// typically displayed using `printf("%08X", id)`
	uint32_t id;
	/// item level
	uint8_t level;
	/// see the #d2rarity enum
	uint8_t rarity;
	bool multiplePictures;
	uint8_t pictureID;
	bool classSpecific;
	/// only set if classSpecific is true
	/// automagicID = the row in automagic.txt, where the first non-header row
	/// is ID 0, and no rows are skipped when incrementing ID
	uint16_t automagicID;
	/// see #d2lowquality enum
	uint8_t lowQualityID;
	/// related in some way to qualityitems.txt, unsure what the ID <-> row mapping is
	uint8_t superiorID;
	/// magicPrefix = the row in MagicPrefix.txt, where the first non-header row
	/// is ID 1, and only the "Expansion" row is skipped when incrementing ID
	/// (ID 0 is no prefix)
	uint16_t magicPrefix;
	/// magicSuffix = the row in MagicSuffix.txt, where the first non-header row
	/// is ID 1, and only the "Expansion" row is skipped when incrementing ID
	/// (ID 0 is no suffix)
	uint16_t magicSuffix;
	/// setID = the row in SetItems.txt, where the first non-header row
	/// is ID 0, and only the "Expansion" row is skipped when incrementing ID
	uint16_t setID;
	/// uniqueID = the row in UniqueItems.txt, where the first non-header row
	/// is ID 0, and only the "Expansion" row is skipped when incrementing ID
	uint16_t uniqueID;
	/// rare or crafted prefix
	/// nameID1 = the row in RarePrefix.txt, where the first non-header row
	/// is (the max ID in RareSuffix.txt)+1, and no rows are skipped when incrementing ID
	/// for example, with the default txt files:
	/// - RareSuffix.txt's max ID is 155 ('flange')
	///  - therefore, the first non-header row in RarePrefix.txt
	///    ('Beast') would be ID 156
	uint8_t nameID1;
	/// nameID2 = the row in RareSuffix.txt, where the first non-header row
	/// is ID 1, and no rows are skipped when incrementing ID
	/// (ID 0 is no suffix)
	uint8_t nameID2;
	/// list of magic prefixes used by this rare/crafted item (see magicPrefix)
	uint16_t rarePrefixes[D2_MAX_RARE_PREFIXES];
	uint8_t numRarePrefixes;
	/// list of magic suffixes used by this rare/crafted item (see magicSuffix)
	uint16_t rareSuffixes[D2_MAX_RARE_SUFFIXES];
	uint8_t numRareSuffixes;
	/// null-terminated name, not including the 's suffix added by the game
	char personalizedName[D2_MAX_CHAR_NAME_BYTELEN];
	bool timestamp;
	/// the armor value; only set if the item code is in Armor.txt
	uint16_t defenseRating;
	/// only set if the item code has durability (i.e. is in Armor.txt or Weapons.txt)
	/// but can be 0 for items that don't have durability (i.e. phase blade)
	uint8_t maxDurability;
	/// only set if maxDurability > 0
	uint8_t currentDurability;
	/// only set for stackable items (i.e. the stackable column in its .txt is 1)
	uint16_t quantity;
	/// number of total sockets in the item (regardless of their filled state)
	uint8_t numSockets;
	/// list of magic properties, not including set bonuses, runeword properties,
	/// or the properties of any socketed items
	d2itemproplist magicProperties;
	/// List of per-item set bonuses (i.e. green bonuses, not the overall set bonuses).
	/// These are always set even when the bonuses are not active.
	///
	/// NOTE: Not all indexes are valid. Use setBonusesBits to determine which indexes are valid
	///
	/// When the bonuses are active depends on the value of add_func in SetItems.txt
	/// for the setID of the item:
	///
	/// If add_func=2, then it uses the number of items of the set that are worn:
	///  - The property list at index 0 is active when >= 2 items of the set are worn.
	///  - The property list at index 1 is active when >= 3 items of the set are worn.
	///  - etc.
	///
	/// If add_func=1, then specific other items of the set need to be worn:
	///  - If the item's setID is the first of the set:
	///   + then the property list at index 0 is active when the second setID of the set is worn
	///   + and the property list at index 1 is active when the third setID of the set is worn
	///   + etc.
	///  - If the item's setID is the second of the set:
	///   + then the property list at index 0 is active when the first setID of the set is worn
	///   + and the property list at index 1 is active when the third setID of the set is worn
	///   + etc.
	d2itemproplist setBonuses[D2_MAX_SET_PROPERTIES];
	/// Bit field containing the position of valid elements in the setBonuses array.
	///
	/// - If bit 0 is set, then setBonuses will have a valid d2itemproplist at index 0.
	/// - If bit 1 is set, then setBonuses will have a valid d2itemproplist at index 1.
	/// - etc.
	uint8_t setBonusesBits;
	/// list of magic properties added to the item via a runeword (see also `isRuneword`)
	d2itemproplist runewordProperties;

};

/**
* Parse the item (+ any socketed items within) in `filename`, and store the result in `item`
*
* @param item A pointer an uninitialized d2item object.
*              If this function returns `D2ERR_OK`, then `item` will need to be cleaned up with d2item_destroy().
*              If this function returns something other than `D2ERR_OK`, then `item` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the item.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*
* @see d2item_destroy(), d2item_parse(), d2item_parse_single()
*/
CHECK_RESULT d2err d2item_parse_file(const char* filename, d2item* item, d2gamedata *gameData, size_t* out_bytesRead);
/**
* Parse the item (+ any socketed items within) in `data` starting at `startByte`, and store the result in `item`
*
* @param item A pointer an uninitialized d2item object.
*              If this function returns `D2ERR_OK`, then `item` will need to be cleaned up with d2item_destroy().
*              If this function returns something other than `D2ERR_OK`, then `item` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the item.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*
* @see d2item_destroy(), d2item_parse_file(), d2item_parse_single()
*/
CHECK_RESULT d2err d2item_parse(const unsigned char* const data, size_t dataSizeBytes, size_t startByte, d2item* item, d2gamedata *gameData, size_t* out_bytesRead);
/**
* Parse the item (but not socketed items within) in `data` starting at `startByte`, and store the result in `item`
*
* If the item has items socketed in it, `item->socketedItems` will be initialized with size `item->numItemsInSockets`, but will be empty.
*
* @param item A pointer an uninitialized d2item object.
*             If this function returns `D2ERR_OK`, then `item` will need to be cleaned up with d2item_destroy().
*             If this function returns something other than `D2ERR_OK`, then `item` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the item.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*
* @see d2item_destroy(), d2item_parse_file(), d2item_parse()
*/
CHECK_RESULT d2err d2item_parse_single(const unsigned char* const data, size_t dataSizeBytes, size_t startByte, d2item* item, d2gamedata *gameData, size_t* out_bytesRead);
CHECK_RESULT d2err d2item_copy(d2item* dest, const d2item* const src);
void d2item_destroy(d2item *item);

typedef struct d2stashpage {
	uint32_t pageNum;
	uint32_t flags;
	char name[D2_MAX_STASH_PAGE_NAME_BYTELEN];
} d2stashpage;

typedef struct d2sharedstash_info {
	uint16_t fileVersion;
	uint32_t sharedGold;
	uint32_t expectedNumPages;
} d2sharedstash_info;

/// PlugY Shared Stash (.sss)
typedef struct d2sharedstash {
	d2itemlist* itemsByPage;
	d2stashpage* pages;
	uint32_t numPages;
	d2sharedstash_info info;
} d2sharedstash;

/**
* Parse the PlugY shared stash in `filename`, and store the result in `stash`
*
* @param stash A pointer to an uninitialized d2sharedstash object.
*             If this function returns `D2ERR_OK`, then `stash` will need to be cleaned up with d2sharedstash_destroy().
*             If this function returns something other than `D2ERR_OK`, then `stash` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the stash.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*/
CHECK_RESULT d2err d2sharedstash_parse_file(const char* filename, d2sharedstash *stash, d2gamedata *gameData, size_t* out_bytesRead);
CHECK_RESULT d2err d2sharedstash_parse(const unsigned char* const data, size_t dataSizeBytes, d2sharedstash *stash, d2gamedata *gameData, size_t* out_bytesRead);
void d2sharedstash_destroy(d2sharedstash *stash);

typedef struct d2personalstash_info {
	uint16_t fileVersion;
	uint32_t expectedNumPages;
} d2personalstash_info;

/// PlugY Personal Stash (.d2x)
typedef struct d2personalstash {
	d2itemlist* itemsByPage;
	d2stashpage* pages;
	uint32_t numPages;
	d2personalstash_info info;
} d2personalstash;

/**
* Parse the PlugY personal stash in `filename`, and store the result in `stash`
*
* @param stash A pointer to an uninitialized d2personalstash object.
*             If this function returns `D2ERR_OK`, then `stash` will need to be cleaned up with d2personalstash_destroy().
*             If this function returns something other than `D2ERR_OK`, then `stash` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the stash.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*/
CHECK_RESULT d2err d2personalstash_parse_file(const char* filename, d2personalstash *stash, d2gamedata *gameData, size_t* out_bytesRead);
CHECK_RESULT d2err d2personalstash_parse(const unsigned char* const data, size_t dataSizeBytes, d2personalstash *stash, d2gamedata *gameData, size_t* out_bytesRead);
void d2personalstash_destroy(d2personalstash *stash);

typedef struct d2char_info {
	bool isExpansion;
	uint32_t fileVersion;
	uint32_t mercID;
	bool isDead;
} d2char_info;

/// Character Save File (.d2s)
typedef struct d2char {
	d2itemlist items;
	d2itemlist itemsCorpse;
	d2itemlist itemsMerc;
	d2char_info info;
} d2char;

/**
* Parse the character in `filename`, and store the result in `character`
*
* @param character A pointer to an uninitialized d2char object.
*             If this function returns `D2ERR_OK`, then `character` will need to be cleaned up with d2char_destroy().
*             If this function returns something other than `D2ERR_OK`, then `character` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the character.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*/
CHECK_RESULT d2err d2char_parse_file(const char* filename, d2char *character, d2gamedata *gameData, size_t* out_bytesRead);
CHECK_RESULT d2err d2char_parse(const unsigned char* const data, size_t dataSizeBytes, d2char *character, d2gamedata *gameData, size_t* out_bytesRead);
void d2char_destroy(d2char *character);

typedef struct d2atmastash_info {
	uint16_t fileVersion;
	uint16_t expectedNumItems;
} d2atmastash_info;

/// ATMA Stash (.d2x)
typedef struct d2atmastash {
	d2itemlist items;
	d2atmastash_info info;
} d2atmastash;

/**
* Parse the d2x ATMA stash in `filename`, and store the result in `stash`
*
* @param stash A pointer to an uninitialized d2atmastash object.
*              If this function returns `D2ERR_OK`, then `stash` will need to be cleaned up with d2atmastash_destroy().
*              If this function returns something other than `D2ERR_OK`, then `stash` will remain uninitialized.
* @param out_bytesRead On `D2ERR_OK`, set to the number of bytes read when parsing the stash.
*                      On error, set to the number of bytes successfully parsed before the error.
* @return `D2ERR_OK` on success
*/
CHECK_RESULT d2err d2atmastash_parse_file(const char* filename, d2atmastash* stash, d2gamedata *gameData, size_t* out_bytesRead);
CHECK_RESULT d2err d2atmastash_parse(const unsigned char* const data, size_t dataSizeBytes, d2atmastash* stash, d2gamedata *gameData, size_t* out_bytesRead);
void d2atmastash_destroy(d2atmastash* stash);

typedef enum d2itemreader_parse_state {
	PARSE_STATE_NOTHING_PARSED = 0,
	PARSE_STATE_ITEMLIST_READY,
	PARSE_STATE_ITEM_READY,
	PARSE_STATE_ITEMLIST_DONE,
	PARSE_STATE_PAGE_READY,
	PARSE_STATE_NEEDS_VERIFICATION,
	PARSE_STATE_FINISHED,
	PARSE_STATE_NONE,
} d2itemreader_parse_state;

typedef enum d2char_section {
	D2CHAR_SECTION_MAIN,
	D2CHAR_SECTION_CORPSE,
	D2CHAR_SECTION_MERC,
	D2CHAR_SECTION_GOLEM,
} d2char_section;

typedef struct d2itemreader_state {
	d2itemreader_parse_state parseState;
	uint32_t curPage;
	uint32_t numPages;
	uint16_t curItem;
	uint16_t numItems;
	size_t lastItemSize;
} d2itemreader_state;

typedef struct d2itemreader_source {
	const unsigned char* data;
	size_t dataSizeBytes;
	size_t curByte;
	// FIXME: this is temporary, should probably use something different
	bool dataNeedsFree;
} d2itemreader_source;

typedef struct d2itemreader_stream {
	d2itemreader_source source;
	d2gamedata* gameData;
	d2err err;
	d2filetype filetype;
	d2itemreader_state state;
	union {
		d2char_info d2char;
		d2atmastash_info d2atmastash;
		d2sharedstash_info d2sharedstash;
		d2personalstash_info d2personalstash;
	} info;
	d2stashpage curPage;
	d2char_section curSection;
} d2itemreader_stream;

CHECK_RESULT d2err d2itemreader_open_file(d2itemreader_stream* stream, const char* filepath, d2gamedata* gameData);
CHECK_RESULT d2err d2itemreader_open_buffer(d2itemreader_stream* stream, const unsigned char* const data, size_t dataSizeBytes, d2gamedata* gameData);
void d2itemreader_close(d2itemreader_stream* stream);
unsigned char* const d2itemreader_dump_last_item(d2itemreader_stream* stream, size_t* out_itemSizeBytes);

/*
* Get the byte position of the d2itemreader_stream (useful for printing the location of an error)
*/
size_t d2itemreader_bytepos(d2itemreader_stream* stream);

/**
* Get the next item in the `stream`
*
* @param stream A pointer to an open d2itemreader_stream.
* @param item If this function returns `true`, set to the parsed item.
*             If this function returns `false`, `*item` remains uninitialized.
* @return `true` on success, `false` on error or no more items.
*         On error, `stream->err != D2ERR_OK`.
*         On no more items, `stream->parseState == PARSE_STATE_FINISHED`.
*/
CHECK_RESULT bool d2itemreader_next(d2itemreader_stream* stream, d2item* item);

/**
* Get the next item in the `stream` but stop if the `stopOn` parse state is hit at any point.
*
* @param stream A pointer to an open d2itemreader_stream.
* @param item If this function returns `true`, set to the parsed item.
*             If this function returns `false`, `*item` remains uninitialized.
* @param stopOn The parse state to check for.
* @return `true` on success, `false` on error, no more items, or `stopOn` being hit.
*         On error, `stream->err != D2ERR_OK`.
*         On no more items, `stream->parseState == PARSE_STATE_FINISHED`.
*         On stop, `stream->parseState == onStop`.
*/
CHECK_RESULT bool d2itemreader_next_but_stop_on(d2itemreader_stream* stream, d2item* item, d2itemreader_parse_state stopOn);

/**
* Seek the `stream` until the specified `state` is hit or an error occurs.
* Will never seek past `PARSE_STATE_ITEM_READY` or `PARSE_STATE_FINISHED` states.
*
* @param stream A pointer to an open d2itemreader_stream.
* @param state The parse state to check for.
* @return `true` on success, `false` on error while seeking
*/
CHECK_RESULT bool d2itemreader_seek_parse_state(d2itemreader_stream* stream, d2itemreader_parse_state state);

/**
* Seek the `stream` until it is ready to read a valid item.
* Similar to `d2itemreader_seek_parse_state` but will skip over empty item lists.
*
* @param stream A pointer to an open d2itemreader_stream.
* @return `true` on success, `false` on error while seeking
*/
CHECK_RESULT bool d2itemreader_seek_valid_item(d2itemreader_stream* stream);

#ifdef __cplusplus
}
#endif

#endif
