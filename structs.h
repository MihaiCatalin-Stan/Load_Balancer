/* Copyright 2023 <Stan Mihai-Catalin> */
#ifndef STRUCTS_H_
#define STRUCTS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define MAX_STRING_SIZE	256
#define HMAX 100

typedef struct info info;
struct info {
	void *key;
	void *value;
};

/*
 * Structura de nod contine:
 * 1) Un pointer catre datele stocate in acesta
 * 2) Un pointer catre urmatorul nod din lista
 */
typedef struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
} ll_node_t;

/*
 * Structura de lista simplu-inlantuita contine:
 * 1) Un pointer catre primul nod din lista
 * 2) Dimensiunea datelor stocate in lista
 * 3) Numarul de noduri din lista
 */
typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

/*
 * Structura de hashtable contine:
 * 1) Un array de liste simplu-inlantuite
 * 2) Numarul de entry-uri in hashtable
 * 3) Numarul maxim de bucket-uri
 * 4) Pointeri catre urmatoarele functii:
 * 4.1) Functia de calcul a valorii hash pentru o cheie asociata
 * 4.2) Functie de comparatie a valorilor hash-urilor a doua chei
 * 4.3) Functie de free a unei perechi cheie-valoare din hashtable
 */
typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets;
	unsigned int size;
	unsigned int hmax;
	unsigned int (*hash_function)(void*);
	int (*compare_function)(void*, void*);
	void (*key_val_free_function)(void*);
};

/*
 * Structura de server_memory contine:
 * 1) Un pointer catre hashtable-ul asociat serverului curent
 * 2) ID-ul serverului curent
 * (sau in cazul unei replici, ID-ul replicii)
 * 3) Hash-ul asociat serverului
 * (sau in cazul unei replici, hash-ul asociat replicii)
 */
struct server_memory {
	hashtable_t *ht;
	unsigned int server_id;
	unsigned int server_hash;
};
typedef struct server_memory server_memory;

/*
 * Structura de load_balancer contine:
 * 1) Numarul de servere curente din hashring
 * 2) Un array de pointeri catre serverele de pe hashring
 * 3) Pointeri catre urmatoarele functii:
 * 3.1) Functia de calcul a valorii hash asociata ID-ului unui server
 * 3.2) Functia de comparatie a valorilor hash-urilor a doua servere
 * 3.3) Functia de calcul a valorii hash asociata unei key
 */
typedef struct load_balancer load_balancer;
struct load_balancer {
	int server_num;
	server_memory **servers;
	unsigned int (*hash_server)(void *);
	int (*compare_function)(void *, void *);
	unsigned int (*hash_key)(void *);
};

#endif /* STRUCTS_H_ */
