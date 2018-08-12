#ifndef STRSET_H_
#define STRSET_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>

typedef size_t(*strset_hash_fn)(const unsigned char*);
typedef struct strset_t strset_t;

strset_t* strset_new(size_t numBuckets, strset_hash_fn hash_fn);
void strset_free(strset_t* set);

int strset_has(strset_t* set, const char* str);
void strset_put(strset_t* set, const char* str);
void strset_iterate(strset_t* set, void(*callback)(const char* str, void* context), void* context);
size_t strset_count(strset_t* set);

size_t strset_hash_default(const unsigned char* key);

#ifdef __cplusplus
}
#endif

#endif
