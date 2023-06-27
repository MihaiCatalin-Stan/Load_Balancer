/* Copyright 2023 <Stan Mihai-Catalin> */
#include "lists.h"
#ifndef HASHTABLES_H_
#define HASHTABLES_H_

/*
 * Functii de comparatie a cheilor, dupa tipul asociat cheii
 * In cazul nostru vom avea functia de ints asociata load_balancer-ului
 * si functia de strings asociata hashtable-ului
 */
int compare_function_ints(void *a, void *b);
int compare_function_strings(void *a, void *b);

/*
 * Functie care intoarce o valoare asociata hash-ului unei valori al carei
 * pointer este trimis ca parametru
 */
unsigned int hash_function_string(void *a);

/*
 * Procedura asociata hashtable-ului pentru a elibera informatia stocata
 * in nodurile din bucket-uri (perechile de date cheie-valoare)
 */
void key_val_free_function(void *data);

/*
 * Functie apelata pentru alocarea si initializarea unui hashtable.
 * Datele primite ca parametrii sunt explicate in sectiunea structs.h
 */
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*));

/*
 * Functie care intoarce valoarea 1 sau 0 pentru urmatoarele cazuri:
 * 1 - daca exista deja o valoare asociata cheii
 * 0 - in caz contrar
 */
int ht_has_key(hashtable_t *ht, void *key);

/*
 * Functie care intoarce valoarea asociata unei chei in hashtable.
 * In cazul in care nu exista deja o valoarea asociata acestei chei se va
 * intoarce valoarea NULL.
 */
void *ht_get(hashtable_t *ht, void *key);

/*
 * Procedura de adaugare a unei perechi key-value in hashtable:
 * ht - pointer catre hashtable-ul in care se va face adaugarea 
 *		perechii key-value
 * key - pointer catre cheia dupa care se va adauga value in hashtable
 * value - valoarea asociata cheii oferite ca parametru
 */
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);

/*
 * Procedura de eliminare a unei perechi key-value din hashtable
 * ht - pointer catre hashtable-ul din care vom scoate informatia
 * key - valoarea cheii asociate informatiei ca trebuie eliminata
 */
void ht_remove_entry(hashtable_t *ht, void *key);

/*
 * Procedura care elibereaza memoria tuturor intrarilor din hashtable,
 * a listelor asociate, a vectorului de pointeri catre liste si a
 * hashtable-ului propriu-zis
 */
void ht_free(hashtable_t *ht);

#endif /* HASHTABLES_H_ */
