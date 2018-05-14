#ifndef D2DATA_H_
#define D2DATA_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "strset.h"

#define D2_ITEM_CODE_STRLEN 4
#define D2_ITEM_CODE_BYTELEN D2_ITEM_CODE_STRLEN+1
#define D2DATA_ITEMSTAT_END_ID 0x1ff
#define D2DATA_MAX_ITEMSTATCOST_IDS D2DATA_ITEMSTAT_END_ID

typedef struct d2data_armor {
	char code[D2_ITEM_CODE_BYTELEN];
} d2data_armor;

typedef struct d2data_weapon {
	char code[D2_ITEM_CODE_BYTELEN];
	bool stackable;
} d2data_weapon;

typedef struct d2data_misc {
	char code[D2_ITEM_CODE_BYTELEN];
	bool stackable;
} d2data_misc;

typedef struct d2data_itemstat {
	uint16_t id;
	uint8_t encode;
	uint16_t saveBits;
	uint16_t saveAdd;
	uint16_t saveParamBits;
	uint16_t nextInChain;
	uint16_t charSaveBits;
} d2data_itemstat;

typedef struct d2data {
	d2data_armor* armors;
	d2data_weapon* weapons;
	d2data_misc* miscs;
	strset_t* armorsSet;
	strset_t* weaponsSet;
	strset_t* stackablesSet;
	d2data_itemstat itemstats[D2DATA_MAX_ITEMSTATCOST_IDS];
} d2data;

bool d2data_is_armor(const char* itemCode, const d2data* data);
bool d2data_is_weapon(const char* itemCode, const d2data* data);
bool d2data_is_stackable(const char* itemCode, const d2data* data);

void d2data_load_armors(const char* txtdata, size_t length, d2data* data);
void d2data_load_armors_from_file(const char* filename, d2data* data);
void d2data_load_weapons(const char* txtdata, size_t length, d2data* data);
void d2data_load_weapons_from_file(const char* filename, d2data* data);
void d2data_load_miscs(const char* txtdata, size_t length, d2data* data);
void d2data_load_miscs_from_file(const char* filename, d2data* data);
void d2data_load_itemstats(const char* txtdata, size_t length, d2data* data);
void d2data_load_itemstats_from_file(const char* filename, d2data* data);

void d2data_destroy(d2data* data);

#endif
