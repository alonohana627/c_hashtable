#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hashtable.h"
#include "../include/prime.h"

static ht_item HT_DELETED_ITEM = {NULL, NULL}; /* Fast delete */
static const int HT_PRIME_1 = 151;
static const int HT_PRIME_2 = 163;

/*
 * Initialises a new item containing k: v
 */
static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* item = malloc(sizeof(ht_item));
    item->key = strdup(k);
    item->value = strdup(v);
    return item;
}

/*
 * Deletes the ht_item item
 */
static void ht_free_item(ht_item* item) {
    free(item->key);
    free(item->value);
    free(item);
}

/*
 * Initialises a new empty hash table
 */
ht_hash_table* ht_new() {
    return ht_new_sized(0);
}


/*
 * Initialises a new empty hash table using a particular size index
 */
static ht_hash_table* ht_new_sized(const int size_index) {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht->size_index = size_index;

    const int base_size = 50 << ht->size_index;
    ht->size = next_prime(base_size);

    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

/*
 * Deletes the hash table
 */
void ht_free_hash_table(ht_hash_table* ht) {
    int i;
    for (i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_free_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

/*
 * Resize the Hash Table.
 * Not needed for the project (since all of the data is needed and we don't wond to delete anything)
 * but it is useful for other applications.
 */
static void ht_resize(ht_hash_table* ht, const int direction) {
    const int new_size_index = ht->size_index + direction;
    int i;

    if (new_size_index < 0) {
        return;
    }

    /*Creates new temp ht*/
    ht_hash_table* new_ht = ht_new_sized(new_size_index);

    /* Iterate through existing hash table, add all items to new*/
    for (i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) { /*Skips deleted or non-existed key-values*/
            ht_insert(new_ht, item->key, item->value);
        }
    }

    /* Pass new_ht and ht's properties. Delete new_ht*/
    ht->size_index = new_ht->size_index;
    ht->count = new_ht->count;

    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_free_hash_table(new_ht);
}

/*
 * This is the main hashing function
 */
static int ht_generic_hash(const char* s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    int i;
    for (i = 0; i < len_s; i++) {
        /* map a char to a large integer */ 
        hash += (long) pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}

/*Double hashing*/
static int ht_hash(const char* s, const int num_buckets, const int attempt) {
    const int hash_a = ht_generic_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_generic_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}


/*
 * Inserts the 'key': 'value' pair into the hash table
 */
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    /* Resize if load factor > 70% */
    const int load = ht->count * 100 / ht->size;
    int i = 1;

    if (load > 70) {
        ht_resize(ht, 1);
    }

    ht_item* item = ht_new_item(key, value);
    int index = ht_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];

    while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
        if (strcmp(cur_item->key, key) == 0) {
            ht_free_item(cur_item);
            ht->items[index] = item;
            return;
        }

        index = ht_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }

    ht->items[index] = item;
    ht->count++;
}


/*
 * Returns the value associated with 'key', or NULL if the key doesn't exist
 */
char* ht_search(ht_hash_table* ht, const char* key) {
    int index = ht_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;

    while (item != NULL && item != &HT_DELETED_ITEM) {
        if (strcmp(item->key, key) == 0) {
            return item->value;
        }
        index = ht_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    } 

    return NULL;
}


/*
 * Deletes key's item from the hash table. Does nothing if 'key' doesn't exist
 * No needed for the project, but we put it might be useful for other applications.
 */
void ht_delete(ht_hash_table* ht, const char* key) {
    /* Resize if load < 10%*/
    const int load = ht->count * 100 / ht->size;

    if (load < 10) {
        ht_resize(ht, -1);
    }

    int index = ht_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;

    while (item != NULL && item != &HT_DELETED_ITEM) { /*while item isn't null or isn't a deleted one*/
        if (strcmp(item->key, key) == 0) {
            ht_free_item(item);
            ht->items[index] = &HT_DELETED_ITEM; /*Marked as deleted*/
        }

        index = ht_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    } 
    ht->count--;
}
