#ifndef D2CONST_H_
#define D2CONST_H_

#ifdef __cplusplus
extern "C" {
#endif

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
#define D2_ITEM_CODE_STRLEN 4
#define D2_ITEM_CODE_BYTELEN D2_ITEM_CODE_STRLEN+1

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

#ifdef __cplusplus
}
#endif

#endif