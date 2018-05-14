#include "strset.h"
#include <string.h>

#define STRSET_BUCKET_INITIAL_SIZE 1
#define STRSET_BUCKET_RESIZE_BITS 2

typedef struct strset_bucket
{
	size_t count;
	size_t size;
	char** values;
} strset_bucket;

typedef struct strset_t
{
	size_t numBuckets;
	strset_bucket* buckets;
	strset_hash_fn hash_fn;
} strset_t;

strset_t* strset_new(size_t numBuckets, strset_hash_fn hash_fn)
{
	strset_t* set = malloc(sizeof(strset_t));
	if (set == NULL)
	{
		return NULL;
	}
	set->numBuckets = numBuckets;
	set->hash_fn = hash_fn;
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
	size_t hash = (*set->hash_fn)(str);
	return &set->buckets[hash % set->numBuckets];
}

int strset_has(strset_t* set, const char* str)
{
	if (set == NULL || str == NULL) return 0;

	strset_bucket* bucket = strset_get_bucket(set, str);
	return strset_bucket_has(bucket, str);
}

void strset_put(strset_t* set, const char* str)
{
	if (set == NULL || str == NULL) return;

	strset_bucket* bucket = strset_get_bucket(set, str);
	if (strset_bucket_has(bucket, str))
	{
		return;
	}

	// resize the bucket if needed
	if (bucket->count == bucket->size)
	{
		bucket->size = bucket->size > 0 ? bucket->size << STRSET_BUCKET_RESIZE_BITS : STRSET_BUCKET_INITIAL_SIZE;
		bucket->values = realloc(bucket->values, bucket->size * sizeof(*bucket->values));
	}

	// copy the string and put it in the bucket
	char* value = malloc(strlen(str) + 1);
	bucket->values[bucket->count] = strcpy(value, str);
	bucket->count++;
}

// djb2 from http://www.cse.yorku.ca/~oz/hash.html
size_t strset_hash_default(const unsigned char* key)
{
	size_t hash = 5381;
	int c;

	while (c = *key++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}
