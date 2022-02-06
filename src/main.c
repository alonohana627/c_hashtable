#include "../include/hashtable.h"
#include <stdio.h>

/*Simple runner*/
int main(){
    ht_hash_table* table = ht_new();
    ht_insert(table, "key", "value");
    ht_insert(table, "key2", "value2");
    ht_insert(table, "key3", "value3");
    ht_insert(table, "key4", "value2");
    ht_insert(table, "key7", "value3");

    printf("%s\n", ht_search(table, "key"));
    printf("%s\n", ht_search(table, "key2"));
    printf("%s\n", ht_search(table, "key3"));

    ht_delete(table, "key");

    printf("%s\n", ht_search(table, "key"));

    ht_free_hash_table(table);
}
