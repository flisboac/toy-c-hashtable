#include <stddef.h>

typedef struct HashTable_Entry_t {

	void *key;
	void *obj;
} HashTable_Entry;

typedef struct HashTable_t HashTable;
typedef size_t (*HashTable_hash_f)(void* key);
typedef void (*HashTable_collect_f)(void* ptr, HashTable *H, void *ptr);
typedef void* (*HashTable_alloc_f)(void *ptr, size_t sz);

HashTable* HashTable_new();
HashTable* HashTable_newWith(size_t cap, size_t load, HashTable_hash_f hash, HashTable_alloc_f alloc);
int HashTable_free(HashTable* H, HashTable_collect_f freefn);
int HashTable_put(HashTable* H, void* key, void* obj);
void* HashTable_get(HashTable* H, void* key);
void* HashTable_remove(HashTable* H, void* key);
int HashTable_has(HashTable* H, void* key);
int HashTable_resize(HashTable* H, size_t cap);
void HashTable_swapcontent(HashTable *H, HashTable *O);

size_t HashTable_voidphashf(void* key);
size_t HashTable_getcap(HashTable *H);
size_t HashTable_getload(HashTable *H);
void HashTable_setautoresize(HashTable *H, unsigned int mult);
HashTable_hash_f HashTable_gethashf(HashTable *H);
size_t HashTable_collectentries(HashTable *H, HashTable_collect_f fn);
size_t HashTable_collectobjects(HashTable *H, HashTable_collect_f fn);
