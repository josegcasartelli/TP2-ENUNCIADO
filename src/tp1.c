#include "tp1.h"
#include "aux.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tp1 {
	struct pokemon *pokemones;
	size_t cantidad_pokemones;
};

void liberar_memoria(struct pokemon *pokemones, size_t cantidad);

struct pokemon *parsear_pokemon(char *linea)
{
	if (!linea)
		return NULL;

	struct pokemon *p = malloc(sizeof(struct pokemon));
	if (!p)
		return NULL;

	char *primer_coma = strchr(linea, ',');
	if (!primer_coma) {
		free(p);
		return NULL;
	}

	char *segunda_coma = strchr(primer_coma + 1, ',');
	if (!segunda_coma) {
		free(p);
		return NULL;
	}

	ptrdiff_t largo_nombre_diff = segunda_coma - (primer_coma + 1);
	if (largo_nombre_diff < 0) {
		free(p);
		return NULL;
	}

	size_t largo_nombre = (size_t)largo_nombre_diff;

	p->nombre = malloc(largo_nombre + 1);
	if (!p->nombre) {
		free(p);
		return NULL;
	}

	for (size_t i = 0; i < largo_nombre; i++) {
		p->nombre[i] = primer_coma[1 + i];
	}

	p->nombre[largo_nombre] = '\0';

	char *tercera_coma = strchr(segunda_coma + 1, ',');
	if (!tercera_coma) {
		free(p->nombre);
		free(p);
		return NULL;
	}

	ptrdiff_t largo_tipo_diff = tercera_coma - (segunda_coma + 1);
	if (largo_tipo_diff < 0) {
		free(p->nombre);
		free(p);
		return NULL;
	}

	size_t largo_tipo = (size_t)largo_tipo_diff;

	char *tipo_str = malloc(largo_tipo + 1);
	if (!tipo_str) {
		free(p->nombre);
		free(p);
		return NULL;
	}

	strncpy(tipo_str, segunda_coma + 1, largo_tipo);
	tipo_str[largo_tipo] = '\0';

	int campos = sscanf(linea, "%d,%*[^,],%*[^,],%d,%d,%d", &p->id,
			    &p->ataque, &p->defensa, &p->velocidad);

	if (campos != 4) {
		free(p->nombre);
		free(tipo_str);
		free(p);
		return NULL;
	}

	if (strcmp(tipo_str, "ELEC") == 0) {
		p->tipo = TIPO_ELEC;
	} else if (strcmp(tipo_str, "FUEG") == 0) {
		p->tipo = TIPO_FUEG;
	} else if (strcmp(tipo_str, "PLAN") == 0) {
		p->tipo = TIPO_PLAN;
	} else if (strcmp(tipo_str, "AGUA") == 0) {
		p->tipo = TIPO_AGUA;
	} else if (strcmp(tipo_str, "NORM") == 0) {
		p->tipo = TIPO_NORM;
	} else if (strcmp(tipo_str, "FANT") == 0) {
		p->tipo = TIPO_FANT;
	} else if (strcmp(tipo_str, "PSI") == 0) {
		p->tipo = TIPO_PSI;
	} else if (strcmp(tipo_str, "LUCH") == 0) {
		p->tipo = TIPO_LUCH;
	} else {
		free(p->nombre);
		free(tipo_str);
		free(p);
		return NULL;
	}

	free(tipo_str);
	return p;
}

tp1_t *tp1_leer_archivo(const char *nombre)
{
	if (!nombre) {
		return NULL;
	}
	FILE *archivo = fopen(nombre, "r");
	if (!archivo) {
		return NULL;
	}

	struct pokemon *pokemones = NULL;
	size_t cantidad = 0;

	char *linea;
	bool error = false;
	while ((linea = leer_linea(archivo)) != NULL && !error) {
		struct pokemon *p = parsear_pokemon(linea);
		free(linea);

		if (p) {
			struct pokemon *aux =
				realloc(pokemones, sizeof(struct pokemon) *
							   (cantidad + 1));
			if (aux) {
				pokemones = aux;
				pokemones[cantidad] = *p;

				pokemones[cantidad].nombre =
					malloc(strlen(p->nombre) + 1);
				if (!pokemones[cantidad].nombre) {
					free(p->nombre);
					free(p);
					liberar_memoria(pokemones, cantidad);
					error = true;
				} else {
					strcpy(pokemones[cantidad].nombre,
					       p->nombre);
					free(p->nombre);
					free(p);
					cantidad++;
				}

			} else {
				liberar_memoria(pokemones, cantidad);
				free(p->nombre);
				free(p);
				error = true;
			}
		}
	}
	fclose(archivo);

	if (cantidad == 0 || error) {
		liberar_memoria(pokemones, cantidad);
		return NULL;
	}

	tp1_t *tp1 = malloc(sizeof(tp1_t));
	if (!tp1) {
		liberar_memoria(pokemones, cantidad);
		return NULL;
	}

	struct pokemon *pokemones_no_repetidos =
		malloc(sizeof(struct pokemon) * cantidad);
	if (!pokemones_no_repetidos) {
		liberar_memoria(pokemones, cantidad);
		free(tp1);
		return NULL;
	}

	size_t total = 0;

	for (size_t i = 0; i < cantidad; i++) {
		bool id_repetido = false;

		for (size_t j = 0; j < total; j++) {
			if (pokemones_no_repetidos[j].id == pokemones[i].id) {
				id_repetido = true;
			}
		}

		if (!id_repetido) {
			pokemones_no_repetidos[total] = pokemones[i];

			pokemones_no_repetidos[total].nombre =
				malloc(strlen(pokemones[i].nombre) + 1);
			if (!pokemones_no_repetidos[total].nombre) {
				liberar_memoria(pokemones_no_repetidos,
						cantidad);
				free(tp1);
				return NULL;
			}

			strcpy(pokemones_no_repetidos[total].nombre,
			       pokemones[i].nombre);
			total++;
		}
	}

	liberar_memoria(pokemones, cantidad);

	tp1->pokemones = pokemones_no_repetidos;
	tp1->cantidad_pokemones = total;

	return tp1;
}

size_t tp1_con_cada_pokemon(tp1_t *un_tp, bool (*f)(struct pokemon *, void *),
			    void *extra)
{
	if (!un_tp || !f)
		return 0;

	size_t cuantos_aplicados = 0;
	for (size_t i = 0; i < un_tp->cantidad_pokemones; i++) {
		if (!f(&un_tp->pokemones[i], extra))
			break;
		cuantos_aplicados++;
	}

	return cuantos_aplicados;
}

void tp1_destruir(tp1_t *tp1)
{
	if (!tp1)
		return;

	for (size_t i = 0; i < tp1->cantidad_pokemones; i++) {
		free(tp1->pokemones[i].nombre);
	}

	free(tp1->pokemones);
	free(tp1);
}

void liberar_memoria(struct pokemon *pokemones, size_t cantidad)
{
	for (size_t i = 0; i < cantidad; i++) {
		free(pokemones[i].nombre);
	}
	free(pokemones);
}