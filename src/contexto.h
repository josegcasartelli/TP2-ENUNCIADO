#ifndef CONTEXTO_H
#define CONTEXTO_H

#include <stdbool.h>
#include "tp1.h"
#include "pokedex.h"

typedef struct contexto {
	pokedex_t *pokedex;
	bool salir;
	int estilo_menu;
} contexto_t;

contexto_t contexto_crear();

#endif
