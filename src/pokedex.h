#ifndef POKEDEX_H
#define POKEDEX_H

#include "hash.h"
#include "abb.h"
#include "tp1.h"

typedef struct pokedex pokedex_t;

pokedex_t *pokedex_crear();

bool pokedex_insertar(pokedex_t *pokedex, struct pokemon *p);

struct pokemon *pokedex_buscar_por_nombre(pokedex_t *pokedex,
					  const char *nombre);

struct pokemon *pokedex_buscar_por_id(pokedex_t *pokedex, int id);

void pokedex_mostrar_por_nombre(pokedex_t *pokedex);

void pokedex_mostrar_por_id(pokedex_t *pokedex);

size_t pokedex_con_cada(pokedex_t *p, bool (*f)(struct pokemon *p, void *extra),
			void *extra);

void destruir_pokemon(void *p);

void pokedex_destruir(pokedex_t *pokedex);

#endif