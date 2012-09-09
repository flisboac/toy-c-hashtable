
#include "hashtable.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_SIZE 16
#define DEFAULT_LOAD 1

typedef struct {

	int using;
	HashTable_Entry entry;
} Bucket;

struct HashTable_t {

	Bucket **table;
	HashTable_hash_f hashf;
	HashTable_alloc_f alloc;
	size_t cap;
	size_t load;
	unsigned int mult;
};


/* ========================================================================== */

static Bucket*
ensureBucketSize(HashTable* H, size_t hash) {

	int i;
	Bucket *buckets = H->table[hash];

	if (!buckets) {
		buckets = H->alloc(NULL, H->load * sizeof(Bucket));
		H->table[hash] = buckets;

		if (buckets) {
			for (i = 0; i < H->load; i++) {
				buckets[i].using = 0;
				buckets[i].entry.key = NULL;
				buckets[i].entry.obj = NULL;
			}
		}
	}

	return buckets;
}


static Bucket*
findFreeBucketEntry(HashTable* H, size_t hash) {

	size_t i;
	int triedResize = 0;
	Bucket *buckets;
	Bucket *bucket = NULL;

	search:
		buckets = ensureBucketSize(H, hash);
		if (buckets) {
			for(i = 0; i < H->load; i++) {
				if (!buckets[i].using) {
					bucket = &buckets[i];
					break;
				}
			}

			if (!bucket && H->mult > 1 && !triedResize) {
				triedResize = 1;
				if (HashTable_resize(H, H->cap * H->mult)) goto search;
			}
		}

	return bucket;
}


static Bucket*
findBucketEntry(HashTable* H, size_t hash, void* key) {

	size_t i;
	Bucket *buckets = ensureBucketSize(H, hash);
	Bucket *bucket = NULL;

	if (buckets) {
		for(i = 0; i < H->load; i++) {
			if (buckets[i].using && buckets[i].entry.key == key) {
				bucket = &buckets[i];
				break;
			}
		}
	}

	return bucket;
}


/* ========================================================================== */


HashTable*
HashTable_new() {
	return HashTable_newwith(DEFAULT_SIZE, DEFAULT_LOAD, NULL, NULL);
}


HashTable*
HashTable_newwith(size_t cap, size_t load, HashTable_hash_f hash, HashTable_alloc_f alloc) {

	int i;
	HashTable* H = NULL;

	if (!alloc) alloc = realloc;
	if (!hash) hash = HashTable_voidphashf;

	if (cap > 0 && load > 0) {
		H = alloc(NULL, sizeof(HashTable));

		if (!H) return NULL;

		H->hashf = hash;
		H->cap = cap;
		H->alloc = alloc;
		H->load = load;
		H->table = alloc(NULL, H->cap * sizeof(Bucket));
		H->mult = 0;

		if (!H->table) {
			alloc(H, 0);
			return NULL;
		}

		for (i = 0; i < H->cap; i++) {
			H->table[i] = NULL;
		}
	}

	return H;
}


void
HashTable_free(HashTable *H) {
	HashTable_freev(H, NULL, NULL);
}


int
HashTable_freev(HashTable* H, HashTable_collect_f freefn, void* state) {
	
	int status = 1;
	int i, j;

	if (H) {

		if (freefn) {
			if (HashTable_collectobjects(H, freefn, state) == 0)
				status = 0;
		}

		for (i = 0; i < H->cap; i++) {
			for (j = 0; j < H->load; j++)
				H->alloc(&(H->table[i][j]), 0);
			H->alloc(H->table[i], 0);
		}

		H->alloc(H, 0);
	}

	return status;
}


int
HashTable_put(HashTable* H, void* key, void* obj) {

	int status = 0;
	int hash;
	Bucket *bucket;

	if (H) {
		hash = H->hashf(key) % H->cap;
		bucket = findBucketEntry(H, hash, key);

		if (bucket) {
			status = 1;
			bucket = NULL;
		} else
			bucket = findFreeBucketEntry(H, hash);

		if (bucket) {
			bucket->using = 1;
			bucket->entry.key = key;
			bucket->entry.obj = obj;
			status = 1;
		}
	}

	return status;
}


void* 
HashTable_get(HashTable* H, void* key) {

	int hash;
	void *obj = NULL;
	Bucket *bucket;

	if (H) {
		hash = H->hashf(key) % H->cap;
		bucket = findBucketEntry(H, hash, key);

		if (bucket) obj = bucket->entry.obj;
	}

	return obj;
}


void* 
HashTable_remove(HashTable* H, void* key) {

	int hash;
	void *obj = NULL;
	Bucket *bucket;

	if (H) {
		hash = H->hashf(key) % H->cap;
		bucket = findBucketEntry(H, hash, key);

		if (bucket) {
			obj = bucket->entry.obj;
			bucket->using = 0;
		}
	}

	return obj;
}


int 
HashTable_has(HashTable* H, void* key) {

	size_t hash;
	int status = 0;
	Bucket *bucket;

	if (H) {
		hash = H->hashf(key) % H->cap;
		bucket = findBucketEntry(H, hash, key);

		if (bucket) status = 1;
	}

	return status;
}


int 
HashTable_resize(HashTable* H, size_t cap) {

	int status = 0, i, j;
	HashTable *O;
	Bucket *bucket;

	/* Perhaps it's not the best way to resize a hashtable, as it
	 * consumes quite a lot of memory to do its job, but at least it doesn't
	 * destroy the original one in case of errors.
	 */

	if (H && cap > H->cap) {
		O = HashTable_newwith(cap, H->load, H->hashf, H->alloc);
		O->mult = H->mult;

		if (O) {
			status = 1;

			/* Copy values */
			for (i = 0; i < H->cap; i++) {
				for (j = 0; j < H->load; j++) {
					bucket = &H->table[i][j];

					if (bucket->using)
						if (!HashTable_put(O, bucket->entry.key, bucket->entry.obj)) {
							status = 0;
							break;
						}
				}
			}

			/* Swap hashtable's internals */
			if (status)	HashTable_swapcontent(H, O);

			HashTable_free(O);
		}
	}

	return status;
}


void
HashTable_swapcontent(HashTable *H, HashTable *O) {
	HashTable T;

	if (H && O) {
		T.table = H->table;
		T.hashf = H->hashf;
		T.alloc = H->alloc;
		T.cap = H->cap;
		T.load = H->load;
		T.mult = H->mult;

		H->table = O->table;
		H->hashf = O->hashf;
		H->alloc = O->alloc;
		H->cap = O->cap;
		H->load = O->load;
		H->mult = O->mult;

		O->table = T.table;
		O->hashf = T.hashf;
		O->alloc = T.alloc;
		O->cap = T.cap;
		O->load = T.load;
		O->mult = T.mult;
	}
}

size_t
HashTable_voidphashf(void* key) {

	size_t hash = 0;
	ptrdiff_t val = (ptrdiff_t)key;

	if (key) {
		hash = val;
		hash ^= (hash >> 16) ^ (hash >> 12);
    	hash = hash ^ (hash >> 7) ^ (hash >> 4);
	}

	return hash;
}


size_t
HashTable_getcap(HashTable *H) {

	size_t cap = 0;

	if (H) {
		cap = H->cap;
	}

	return cap;
}


size_t
HashTable_getload(HashTable *H) {

	size_t load = 0;

	if (H) {
		load = H->load;
	}

	return load;
}

void
HashTable_setautoresize(HashTable *H, unsigned int mult) {

	if (H) {
		if (mult > 1)
			H->mult = mult;
		else
			H->mult = 0;
	}
}

HashTable_hash_f
HashTable_gethashf(HashTable *H) {

	HashTable_hash_f hashf = NULL;

	if (H) {
		hashf = H->hashf;
	}

	return hashf;
}


size_t
HashTable_collectobjects(HashTable *H, HashTable_collect_f fn, void* ptr) {

	HashTable *O = HashTable_newwith(H->cap, 1, HashTable_voidphashf, H->alloc);
	size_t count = 0, fail = 0, i, j;
	Bucket *bucket;

	if (H && O) {

		for (i = 0; i < H->cap; i++) {
			for (j = 0; j < H->load; j++) {
				bucket = &H->table[i][j];

				if (bucket->using) {
					count++;

					if (fn && !HashTable_has(H, bucket->entry.key))
						fn(ptr, H, bucket->entry.key);

					if (fn && !HashTable_has(H, bucket->entry.obj))
						fn(ptr, H, bucket->entry.obj);

					if (!HashTable_put(O, bucket->entry.key, NULL)
							|| !HashTable_put(O, bucket->entry.obj, NULL)) {
						count = 0;
						fail = 1;
						break;
					}
				}
			}

			if (fail) break;
		}
	}

	if (O) {
		HashTable_free(O);
	}

	return count;
}

size_t
HashTable_collectentries(HashTable *H, HashTable_collect_f fn, void* ptr) {

	size_t count = 0, i, j;
	Bucket *bucket;

	if (H) {
		for (i = 0; i < H->cap; i++) {
			for (j = 0; j < H->load; j++) {
				bucket = &H->table[i][j];

				if (bucket->using) {
					count++;

					if (fn) fn(ptr, H, &bucket->entry);
				}
			}
		}
	}

	return count;
}
