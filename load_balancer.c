/* Copyright 2023 <Stan Mihai-Catalin> */
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "load_balancer.h"

unsigned int hash_function_servers(void *a) {
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_key(void *a) {
	unsigned char *puchar_a = (unsigned char *)a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

load_balancer *init_load_balancer() {
	load_balancer *main_server = malloc(sizeof(load_balancer));
	main_server->server_num = 0;
	main_server->hash_server = hash_function_servers;
	main_server->hash_key = hash_function_key;
	main_server->compare_function = compare_function_ints;
	main_server->servers = NULL;
	return main_server;
}

/*
 * Functie care primeste ca parametrii load_balancer-ul si hash-ul care
 * se doreste a fi sortat. Functia itereaza prin vector si returneaza
 * indicele primului hash mai mare decat cel dat.
 */
int new_sort(load_balancer *main, unsigned int hash)
{
	int i = 0;
	for (i = 0; i < main->server_num; i++)
		if (main->servers[i]->server_hash > hash)
			return i;
	return i;
}

/*
 * Procedura de adaugare a unui server si a replicilor sale in load_balancer
 * In cazul primului server se va aloca vectorul de pointeri si se vor adauga
 * pe rand serverele sortate
 * In orice alt caz:
 * 1. Dam realloc la servers si il facem cu 3 pointeri mai mare
 * 2. Verificam cu binary_search pozitia pe care trebuie sa se afle noul server
 * 3. Mutam toate serverele din fata lui cu un element mai in fata
 * 4. Verificam intrarile de pe sv urmator si le mutam pe cele care au hash ul
 * mai mic decat noul server in el
 */
void loader_add_server(load_balancer *main, int server_id) {
	if (!main->servers) {
		main->servers = calloc(3, sizeof(server_memory *));
		for (int i = 0; i < 3; i++)
			main->servers[i] = calloc(1, sizeof(server_memory));
		for (int i = 0; i < 3; i++) {
			server_memory *new_server = init_server_memory();
			new_server->server_id = i * 100000 + server_id;
			new_server->server_hash =
			main->hash_server(&new_server->server_id);
			int pos = 0;
			if (main->servers) {
				pos = new_sort(main, new_server->server_hash);
				if (pos != main->server_num) {
					for (int j = main->server_num; j > pos; j--) {
                        memcpy(main->servers[j], main->servers[j - 1],
							   sizeof(server_memory));
                	}
				}
			}
			memcpy(main->servers[pos], new_server, sizeof(server_memory));
			free(new_server);
			main->server_num++;
		}
	} else {
		main->servers = realloc(main->servers, sizeof(server_memory *) *
								(main->server_num + 3));
		for (int i = main->server_num; i < main->server_num + 3; i++)
			main->servers[i] = calloc(1, sizeof(server_memory));
		for (int i = 0; i < 3; i++) {
			server_memory *new_server = init_server_memory();
			new_server->server_id = i * 100000 + server_id;
			new_server->server_hash =
			main->hash_server(&new_server->server_id);
			int pos = new_sort(main, new_server->server_hash);
			for (int j = main->server_num; j > pos; j--)
                memcpy(main->servers[j], main->servers[j - 1],
					   sizeof(server_memory));
			memcpy(main->servers[pos], new_server, sizeof(server_memory));
			main->server_num++;
			server_memory *next_server =
			main->servers[(pos + 1) % main->server_num];
			for (int j = 0; j < HMAX; j++) {
				linked_list_t *next_list;
				next_list = next_server->ht->buckets[j];
				ll_node_t *cursor = next_list->head;
				for (unsigned int k = 0; k < next_list->size; k++) {
					info *curr_data = cursor->data;
					unsigned int key_hash = main->hash_key(curr_data->key);
					if (key_hash <= main->servers[pos]->server_hash ||
						key_hash > next_server->server_hash)
						server_store(main->servers[pos], curr_data->key,
									 curr_data->value);
					cursor = cursor->next;
				}
			}
			free(new_server);
		}
	}
}

/*
 * Procedura de eliminare a unui server din load_balancer, care primeste ca
 * parametrii load_balancer-ul si id-ul serverului care se doreste a fi scos
 * Functia itereaza de 3 ori (o data pentru server si de 2 ori pentru replici)
 * si apoi executa pe rand:
 * 1) Mutarea elementelor catre serverul urmator in hashring
 * 2) Eliberarea serverului curent
 * 3) Mutarea la stanga pe array al restul serverelor
 * 4) Eliberarea ultimei pozitii din array (redimensionarea propriu-zisa)
 */
void loader_remove_server(load_balancer *main, int server_id) {
	for (int i = 0; i < 3; i++) {
		int new_id = i * 100000 + server_id;
		unsigned int hash = main->hash_server(&new_id);
		int pos = new_sort(main, hash) - 1;
		if (main->servers[pos]->server_hash != hash) {
			printf("Nu exista\n");
			return;
		}
		server_memory *tmp_server = main->servers[pos];
		if (main->servers[(pos + 1) % main->server_num]) {
			server_memory *next_server =
			main->servers[(pos + 1) % main->server_num];
			// parcurgem bucket urile serverului curent
			for (int j = 0; j < HMAX; j++)
				// daca bucket ul are elemente in el
				if (tmp_server->ht->buckets[j]) {
					linked_list_t *tmp_list = tmp_server->ht->buckets[j];
					ll_node_t *cursor = tmp_list->head;
					// parcurgem bucket ul curent
					for (unsigned int k = 0; k < tmp_list->size; k++) {
						server_store(next_server, ((info *)cursor->data)->key,
									 ((info *)cursor->data)->value);
						cursor = cursor->next;
					}
				}
		}
		free_server_memory(tmp_server);
		main->servers[pos] = calloc(1, sizeof(server_memory));
		for (int j = pos; j < main->server_num - 1; j++)
			memcpy(main->servers[j], main->servers[j + 1],
				   sizeof(server_memory));
		free(main->servers[main->server_num - 1]);
		main->server_num--;
	}
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id) {
	unsigned int elem_hash = main->hash_key(key);
	int elem_pos = new_sort(main, elem_hash) % main->server_num;
	*server_id = main->servers[elem_pos]->server_id % 100000;
	server_store(main->servers[elem_pos], key, value);
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
	unsigned int elem_hash = main->hash_key(key);
	int elem_pos = new_sort(main, elem_hash) % main->server_num;
	*server_id = main->servers[elem_pos]->server_id % 100000;
	return server_retrieve(main->servers[elem_pos], key);
}

void free_load_balancer(load_balancer *main) {
	for (int i = 0; i < main->server_num; i++)
		free_server_memory(main->servers[i]);
	free(main->servers);
	free(main);
}
