#include "lista.h"
#include <stddef.h>

struct par {
	char *clave;
	void *valor;
};

typedef struct par par_t;

struct hash {
	lista_t **tabla;
	size_t cantidad;
	size_t capacidad;
};