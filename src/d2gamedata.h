#ifndef D2DATA_H_
#define D2DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "strset.h"
#include "d2err.h"
#include "d2util.h"

/// @private
/// @{
#define D2DATA_ITEMSTAT_END_ID 0x1ff
#define D2DATA_MAX_ITEMSTATCOST_IDS D2DATA_ITEMSTAT_END_ID

#define D2DATA_INIT_STATE_NONE 0
#define D2DATA_INIT_STATE_ARMORS (1 << 0)
#define D2DATA_INIT_STATE_WEAPONS (1 << 1)
#define D2DATA_INIT_STATE_MISCS (1 << 2)
#define D2DATA_INIT_STATE_ITEMSTATS (1 << 3)
#define D2DATA_INIT_STATE_ALL (D2DATA_INIT_STATE_ARMORS|D2DATA_INIT_STATE_WEAPONS|D2DATA_INIT_STATE_MISCS|D2DATA_INIT_STATE_ITEMSTATS)
/// @}

typedef struct d2gamedata_itemstat {
	uint16_t id;
	uint8_t encode;
	uint16_t saveBits;
	int16_t saveAdd;
	uint16_t saveParamBits;
	uint16_t nextInChain;
	uint16_t charSaveBits;
} d2gamedata_itemstat;

typedef struct d2gamedata {
	uint16_t initState;
	strset_t* armorsSet;
	strset_t* weaponsSet;
	strset_t* stackablesSet;
	d2gamedata_itemstat itemstats[D2DATA_MAX_ITEMSTATCOST_IDS];
} d2gamedata;

/// @see d2gamedata_init_files()
typedef struct d2gamedatafiles {
	const char* armorTxtFilepath;
	const char* weaponsTxtFilepath;
	const char* miscTxtFilepath;
	const char* itemStatCostTxtFilepath;
} d2gamedatafiles;

/// @see d2gamedata_init_bufs()
typedef struct d2gamedatabufs {
	char* armorTxt;
	size_t armorTxtSize;
	char* weaponsTxt;
	size_t weaponsTxtSize;
	char* miscTxt;
	size_t miscTxtSize;
	char* itemStatCostTxt;
	size_t itemStatCostTxtSize;
} d2gamedatabufs;

/**
* Load the default data packaged with d2itemreader (should work for un-modded D2 versions >= 1.10)
*
* @return `D2ERR_OK` on success
*
* \attention **IMPORTANT**: d2gamedata_destroy() only needs to be called if the init function returns `D2ERR_OK`
*
* @see d2gamedata_init_files(), d2gamedata_init_bufs(), d2gamedata_destroy()
*/
CHECK_RESULT d2err d2gamedata_init_default(d2gamedata* data);
/**
* Load the data from the file paths given in `files`
*
* @param files Paths to the .txt game data files
* @return `D2ERR_OK` on success
*
* \attention **IMPORTANT**: d2gamedata_destroy() only needs to be called if the init function returns `D2ERR_OK`
*
* @see d2gamedata_init_default(), d2gamedata_init_bufs(), d2gamedata_destroy()
*/
CHECK_RESULT d2err d2gamedata_init_files(d2gamedata* data, d2gamedatafiles files);
/**
* Load the data from the buffers given in `bufs`
*
* @param files Buffers to the .txt game data
* @return `D2ERR_OK` on success
*
* \attention **IMPORTANT**: d2gamedata_destroy() only needs to be called if the init function returns `D2ERR_OK`
*
* @see d2gamedata_init_files(), d2gamedata_init_default(), d2gamedata_destroy()
*/
CHECK_RESULT d2err d2gamedata_init_bufs(d2gamedata* data, d2gamedatabufs bufs);

/**
* Cleanup memory used by d2gamedata.
*
* \attention **IMPORTANT**: d2gamedata_destroy() only needs to be called if the init function returns `D2ERR_OK`
*
* @see d2gamedata_init_default(), d2gamedata_init_files(), d2gamedata_init_bufs()
*/
void d2gamedata_destroy(d2gamedata* data);

/// @private
/// @{
/*
* Initialize an empty d2gamedata struct. Not typically necessary outside of internal library functions
*/
CHECK_RESULT d2err d2gamedata_init(d2gamedata* data);
/// @}

/// @private
/// @{
bool d2gamedata_is_armor(const d2gamedata* data, const char* itemCode);
bool d2gamedata_is_weapon(const d2gamedata* data, const char* itemCode);
bool d2gamedata_is_stackable(const d2gamedata* data, const char* itemCode);

CHECK_RESULT d2err d2gamedata_load_defaults(d2gamedata* data);
CHECK_RESULT d2err d2gamedata_load_armors(d2gamedata* data, const char* txtdata, size_t length);
CHECK_RESULT d2err d2gamedata_load_armors_from_file(d2gamedata* data, const char* filename);
CHECK_RESULT d2err d2gamedata_load_weapons(d2gamedata* data, const char* txtdata, size_t length);
CHECK_RESULT d2err d2gamedata_load_weapons_from_file(d2gamedata* data, const char* filename);
CHECK_RESULT d2err d2gamedata_load_miscs(d2gamedata* data, const char* txtdata, size_t length);
CHECK_RESULT d2err d2gamedata_load_miscs_from_file(d2gamedata* data, const char* filename);
CHECK_RESULT d2err d2gamedata_load_itemstats(d2gamedata* data, const char* txtdata, size_t length);
CHECK_RESULT d2err d2gamedata_load_itemstats_from_file(d2gamedata* data, const char* filename);
/// @}

#ifdef __cplusplus
}
#endif

#endif
