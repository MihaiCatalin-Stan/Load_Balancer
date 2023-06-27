/* Copyright 2023 <Stan Mihai-Catalin> */
#include <stdlib.h>
#include <string.h>

#include "server.h"

server_memory *init_server_memory()
{
	server_memory *server = calloc(1, sizeof(server_memory));
	server->ht = ht_create(HMAX, hash_function_string,
					   compare_function_strings,
					   key_val_free_function);
	server->server_hash = 0;
	server->server_id = 0;
	return server;
}

void server_store(server_memory *server, char *key, char *value) {
	ht_put(server->ht, key, strlen(key), value, strlen(value));
}

char *server_retrieve(server_memory *server, char *key) {
	return ht_get(server->ht, key);
}

void server_remove(server_memory *server, char *key) {
	ht_remove_entry(server->ht, key);
}

void free_server_memory(server_memory *server) {
	ht_free(server->ht);
	free(server);
}
