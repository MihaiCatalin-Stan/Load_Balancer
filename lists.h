/* Copyright 2023 <Stan Mihai-Catalin> */
#include "structs.h"
#ifndef LISTS_H_
#define LISTS_H_

/*
 * Initializam o lista simplu-inlantuita care poate stoca date de dimensiunea
 * data prin parametrul data_size. Functia intoarce lista initializata.
 */
linked_list_t *ll_create(unsigned int data_size);

/*
 * Procedura primeste prin parametrii lista simplu-inlantuita in care se adauga
 * pe pozitia n, un nod nou creat care va retine informatia trimisa prin
 * parametrul de new_data.
 * Indexarea se face de la 0 si trateaza si cazurile de eroare:
 * n >= nr_noduri si n < 0
 */
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data);

/*
 * Functia elimina nodul de pe pozitia n a listei data prin parametrii functiei
 * si intoarce nodul cerut. De asemenea reface legaturile in lista intre nodul
 * precedent si nodul urmator celui eliminat.
 */
ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n);

/*
 * Functia intoarce numarul de noduri a listei primita prin parametrii.
 */
unsigned int ll_get_size(linked_list_t* list);

/*
 * Procedura elibereaza memoria ocupata de nodurile listei pp_list si de
 * datele stocate in acestea. Ulterior elibereaza memoria ocupata de structura
 * si seteaza pointer-ul catre aceasta pe NULL.
 */
void ll_free(linked_list_t** pp_list);

#endif /* LISTS_H_ */
