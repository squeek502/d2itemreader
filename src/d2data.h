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

#define D2DATA_ITEMSTAT_END_ID 0x1ff
#define D2DATA_MAX_ITEMSTATCOST_IDS D2DATA_ITEMSTAT_END_ID

#define D2DATA_INIT_STATE_NONE 0
#define D2DATA_INIT_STATE_ARMORS (1 << 0)
#define D2DATA_INIT_STATE_WEAPONS (1 << 1)
#define D2DATA_INIT_STATE_MISCS (1 << 2)
#define D2DATA_INIT_STATE_ITEMSTATS (1 << 3)
#define D2DATA_INIT_STATE_ALL (D2DATA_INIT_STATE_ARMORS|D2DATA_INIT_STATE_WEAPONS|D2DATA_INIT_STATE_MISCS|D2DATA_INIT_STATE_ITEMSTATS)

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
	uint16_t initState;
	strset_t* armorsSet;
	strset_t* weaponsSet;
	strset_t* stackablesSet;
	d2data_itemstat itemstats[D2DATA_MAX_ITEMSTATCOST_IDS];
} d2data;

bool d2data_is_armor(const char* itemCode, const d2data* data);
bool d2data_is_weapon(const char* itemCode, const d2data* data);
bool d2data_is_stackable(const char* itemCode, const d2data* data);

CHECK_RESULT d2err d2data_load_defaults(d2data* data);
CHECK_RESULT d2err d2data_load_armors(const char* txtdata, size_t length, d2data* data);
CHECK_RESULT d2err d2data_load_armors_from_file(const char* filename, d2data* data);
CHECK_RESULT d2err d2data_load_weapons(const char* txtdata, size_t length, d2data* data);
CHECK_RESULT d2err d2data_load_weapons_from_file(const char* filename, d2data* data);
CHECK_RESULT d2err d2data_load_miscs(const char* txtdata, size_t length, d2data* data);
CHECK_RESULT d2err d2data_load_miscs_from_file(const char* filename, d2data* data);
CHECK_RESULT d2err d2data_load_itemstats(const char* txtdata, size_t length, d2data* data);
CHECK_RESULT d2err d2data_load_itemstats_from_file(const char* filename, d2data* data);

void d2data_destroy(d2data* data);

#ifdef __cplusplus
}
#endif

#endif
