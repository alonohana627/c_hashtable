#ifndef HASHTABLE_H
#define HASHTABLE_H

/*
Key-Value pair
*/
typedef struct ht_item {
    char* key;
    char* value;
} ht_item;

typedef struct {
    int size_index;
    int size;
    int count;
    ht_item** items;
} ht_hash_table;

/*item handling*/
static ht_item* ht_new_item(const char* k, const char* v);
static void ht_free_item(ht_item* item);

/*Hashing*/
static int ht_generic_hash(const char* s, const int a, const int m);
static int ht_hash(const char* s, const int num_buckets, const int attempt);

/*Hash Table*/
ht_hash_table* ht_new();
static ht_hash_table* ht_new_sized(const int size_index);
void ht_free_hash_table(ht_hash_table* ht);
static void ht_resize(ht_hash_table* ht, const int direction);

/*CRUD operations*/
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* h, const char* key); 

#endif 
