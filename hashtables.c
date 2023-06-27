/* Copyright 2023 <Stan Mihai-Catalin> */
#include "hashtables.h"

int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

void key_val_free_function(void *data)
{
	free(((info *)data)->key);
	free(((info *)data)->value);
}

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*))
{
	hashtable_t *ht = malloc(sizeof(hashtable_t));
	ht->size = 0;
	ht->hmax = hmax;
	ht->buckets = malloc(hmax * sizeof(linked_list_t *));
	for (unsigned int i = 0; i < hmax; i++)
		ht->buckets[i] = ll_create(sizeof(info));
	ht->hash_function = hash_function;
	ht->compare_function = compare_function;
	ht->key_val_free_function = key_val_free_function;
	return ht;
}

int ht_has_key(hashtable_t *ht, void *key)
{
	unsigned int pos = ht->hash_function(key) % ht->hmax;
	if (ht->buckets[pos]->head != NULL) {
		ll_node_t *cursor = ht->buckets[pos]->head;
		while (cursor->next != NULL &&
			   (ht->compare_function(key, ((info *)cursor->data)->key)))
			cursor = cursor->next;
		if (!(ht->compare_function(key, ((info *)cursor->data)->key)))
			return 1;
	}
	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	unsigned int pos = ht->hash_function(key) % ht->hmax;
	if (ht->buckets[pos]->head != NULL) {
		ll_node_t *cursor = ht->buckets[pos]->head;
		while ((cursor->next != NULL) &&
			   (ht->compare_function(key, ((info *)cursor->data)->key)))
			cursor = cursor->next;
		if (!(ht->compare_function(key, ((info *)cursor->data)->key)))
			return ((info *)cursor->data)->value;
	}
	return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	unsigned int pos = ht->hash_function(key) % ht->hmax;
	ll_node_t* cursor = ht->buckets[pos]->head;
	info* new_info = malloc(sizeof(info));

	new_info->key = calloc(key_size + 1, sizeof(char));
	memcpy(new_info->key, key, key_size);
	new_info->value = calloc(value_size + 1, sizeof(char));
	memcpy(new_info->value, value, value_size);

	if (!ht_has_key(ht, key)) {
		ll_add_nth_node(ht->buckets[pos], 0, new_info);
		free(new_info);
		ht->size++;
		return;
	}
	while (cursor) {
		info* current_info = (info*)cursor->data;
		if (ht->compare_function(key, current_info->key) == 0) {
			free(current_info->value);
			current_info->value = calloc(value_size + 1, sizeof(char));
			memcpy(current_info->value, new_info->value, value_size);
			free(new_info->key);
			free(new_info->value);
			free(new_info);
			break;
		}
		cursor = cursor->next;
	}
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
	unsigned int pos = ht->hash_function(key) % ht->hmax;
	if (ht_has_key(ht, key) == 0) {
		printf("No se puede\n");
		return;
	}
	ll_node_t *cursor = ht->buckets[pos]->head;
	int key_pos = 0;
	while (ht->compare_function(key, ((info *)cursor->data)->key)) {
		cursor = cursor->next;
		key_pos++;
	}
	key_val_free_function(cursor->data);
	ll_remove_nth_node(ht->buckets[pos], key_pos);
	ht->size--;
}

void ht_free(hashtable_t *ht)
{
	for (unsigned int i = 0; i < ht->hmax; i++) {
		linked_list_t *list_to_free = ht->buckets[i];
		ll_free(&list_to_free);
	}
	free(ht->buckets);
	free(ht);
}
