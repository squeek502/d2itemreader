#include "strset.h"
#include <string.h>

#define STRSET_BUCKET_INITIAL_SIZE 1
#define STRSET_BUCKET_RESIZE_BITS 2

#include <errno.h>
#define STRSET_ERR_OOM -ENOMEM

#if defined(_MSC_VER)
#define inline __inline
#endif

typedef struct strset_bucket
{
	size_t count;
	size_t size;
	char** values;
} strset_bucket;

struct strset_t
{
	size_t numBuckets;
	strset_bucket* buckets;
	strset_hash_fn hash_fn;
	size_t valueCount;
};

strset_t* strset_new(size_t numBuckets, strset_hash_fn hash_fn)
{
	strset_t* set = malloc(sizeof(strset_t));
	if (set == NULL)
	{
		return NULL;
	}
	set->numBuckets = numBuckets;
	set->hash_fn = hash_fn;
	set->valueCount = 0;
	set->buckets = calloc(numBuckets, sizeof(strset_bucket));
	if (set->buckets == NULL)
	{
		free(set);
		return NULL;
	}
	return set;
}

void strset_free(strset_t* set)
{
	if (set == NULL) return;

	for (size_t i = 0; i < set->numBuckets; i++)
	{
		strset_bucket* bucket = &set->buckets[i];
		for (size_t j = 0; j < bucket->count; j++)
		{
			free(bucket->values[j]);
		}
		if (bucket->values != NULL)
			free(bucket->values);
	}
	free(set->buckets);
	free(set);
}

static int strset_bucket_has(strset_bucket* bucket, const char* str)
{
	for (size_t i = 0; i < bucket->count; i++)
	{
		char* value = bucket->values[i];
		if (strcmp(str, value) == 0)
		{
			return 1;
		}
	}
	return 0;
}

static inline strset_bucket* strset_get_bucket(strset_t* set, const char* str)
{
	size_t hash = (*set->hash_fn)((const uint8_t*)str);
	return &set->buckets[hash % set->numBuckets];
}

int strset_has(strset_t* set, const char* str)
{
	if (set == NULL || str == NULL) return 0;

	strset_bucket* bucket = strset_get_bucket(set, str);
	return strset_bucket_has(bucket, str);
}

int strset_put(strset_t* set, const char* str)
{
	if (set == NULL || str == NULL) return 0;

	strset_bucket* bucket = strset_get_bucket(set, str);
	if (strset_bucket_has(bucket, str))
	{
		return 0;
	}

	// resize the bucket if needed
	if (bucket->count == bucket->size)
	{
		size_t newSize = bucket->size > 0 ? bucket->size << STRSET_BUCKET_RESIZE_BITS : STRSET_BUCKET_INITIAL_SIZE;
		void* tmp = realloc(bucket->values, newSize * sizeof(*bucket->values));
		if (tmp == NULL)
		{
			return STRSET_ERR_OOM;
		}
		bucket->size = newSize;
		bucket->values = tmp;
	}

	// copy the string and put it in the bucket
	char* value = strdup(str);
	if (value == NULL)
	{
		return STRSET_ERR_OOM;
	}
	bucket->values[bucket->count] = value;
	bucket->count++;
	set->valueCount++;
	return 1;
}

void strset_iterate(strset_t* set, void(*callback)(const char *str, void* context), void* context)
{
	if (set == NULL || callback == NULL) return;

	for (size_t i = 0; i < set->numBuckets; i++)
	{
		strset_bucket* bucket = &set->buckets[i];
		for (size_t j = 0; j < bucket->count; j++)
		{
			const char* value = bucket->values[j];
			(*callback)(value, context);
		}
	}
}

size_t strset_count(strset_t* set)
{
	return set->valueCount;
}

// djb2 from http://www.cse.yorku.ca/~oz/hash.html
size_t strset_hash_default(const uint8_t* key)
{
	size_t hash = 5381;
	uint8_t c;

	while ((c = *key++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}
