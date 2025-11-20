#include "aux.h"
#include "pokedex.h"
#include <stdlib.h>
#include <string.h>

char *leer_linea(FILE *archivo)
{
	if (!archivo)
		return NULL;

	size_t capacidad = 64;
	char *buffer = malloc(capacidad);
	if (!buffer)
		return NULL;

	size_t len = 0;

	while (fgets(buffer + len, (int)(capacidad - len), archivo)) {
		len += strlen(buffer + len);

		if (len > 0 && buffer[len - 1] == '\n') {
			buffer[len - 1] = '\0';
			return buffer;
		}

		capacidad *= 2;
		char *nuevo = realloc(buffer, capacidad);
		if (!nuevo) {
			free(buffer);
			return NULL;
		}

		buffer = nuevo;
	}

	if (len == 0) {
		free(buffer);
		return NULL;
	}

	return buffer;
}

void limpiar_buffer(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		;
}

char leer_opcion_menu(void)
{
	char c;
	printf("Seleccione una opcion: ");
	if (scanf(" %c", &c) != 1)
		return '\0';
	limpiar_buffer();
	return c;
}

int comparar_por_id(const void *a, const void *b)
{
	const struct pokemon *p1 = a;
	const struct pokemon *p2 = b;
	return p1->id - p2->id;
}

int comparar_por_nombre(const void *a, const void *b)
{
	const struct pokemon *p1 = a;
	const struct pokemon *p2 = b;
	return strcmp(p1->nombre, p2->nombre);
}

static char *duplicar_cadena(const char *s)
{
	size_t len = strlen(s) + 1;
	char *copia = malloc(len);
	if (!copia)
		return NULL;
	memcpy(copia, s, len);
	return copia;
}

bool copiar_pokemon(struct pokemon *original, void *extra)
{
	pokedex_t *pokedex = extra;

	struct pokemon *copia = malloc(sizeof(struct pokemon));
	if (!copia)
		return false;

	memcpy(copia, original, sizeof(struct pokemon));

	copia->nombre = duplicar_cadena(original->nombre);
	if (!copia->nombre) {
		free(copia);
		return false;
	}

	if (!pokedex_insertar(pokedex, copia)) {
		free(copia->nombre);
		free(copia);
		return false;
	}

	return true;
}
