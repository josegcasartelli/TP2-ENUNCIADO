#include "hash.h"
#include "hash_interno.h"
#include "lista.h"
#include "aux.h"
#include <string.h>
#include <stdlib.h>


int comparar_claves(const void *a, const void *b)
{
	const par_t *par1 = a;
	const par_t *par2 = b;
	return strcmp(par1->clave, par2->clave);
}

size_t funcion_hash(char *clave, size_t capacidad)
{
	size_t hash = 5381;
	while (*clave)
		hash = hash * 33 + (unsigned char)(*clave++);
	return hash % capacidad;
}

bool rehash(hash_t *hash)
{
	if (!hash)
		return false;

	size_t nueva_capacidad = hash->capacidad * 2;
	lista_t **nueva_tabla = calloc(nueva_capacidad, sizeof(lista_t *));
	if (!nueva_tabla)
		return false;

	for (size_t i = 0; i < nueva_capacidad; i++) {
		nueva_tabla[i] = lista_crear();
		if (!nueva_tabla[i]) {
			for (size_t j = 0; j < i; j++)
				lista_destruir(nueva_tabla[j]);
			free(nueva_tabla);
			return false;
		}
	}

	for (size_t i = 0; i < hash->capacidad; i++) {
		lista_t *viejo_bucket = hash->tabla[i];
		while (lista_cantidad(viejo_bucket) > 0) {
			par_t *par = lista_eliminar_elemento(
				viejo_bucket, 0);
			size_t nuevo_indice =
				funcion_hash(par->clave, nueva_capacidad);
			if (!lista_agregar(nueva_tabla[nuevo_indice], par)) {
				for (size_t k = 0; k < nueva_capacidad; k++)
					lista_destruir(nueva_tabla[k]);
				free(nueva_tabla);
				return false;
			}
		}
		lista_destruir(viejo_bucket);
	}
	free(hash->tabla);
	hash->tabla = nueva_tabla;
	hash->capacidad = nueva_capacidad;

	return true;
}




hash_t *hash_crear(size_t capacidad_inicial)
{
	if (!capacidad_inicial)
		return NULL;

	if (capacidad_inicial < 3)
		capacidad_inicial = 3;

	hash_t *hash = malloc(sizeof(hash_t));
	if (!hash)
		return NULL;

	hash->capacidad = capacidad_inicial;
	hash->cantidad = 0;

	hash->tabla = calloc(hash->capacidad, sizeof(lista_t *));
	if (!hash->tabla) {
		free(hash);
		return NULL;
	}

	for (size_t i = 0; i < hash->capacidad; i++) {
		hash->tabla[i] = lista_crear();
		if (!hash->tabla[i]) {
			for (size_t j = 0; j < i; j++) {
				lista_destruir(hash->tabla[j]);
			}
			free(hash->tabla);
			free(hash);
			return NULL;
		}
	}

	return hash;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;

	return hash->cantidad;
}

bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado)
{
	if (!hash || !clave)
		return false;

	if ((double)(hash->cantidad + 1) / (double)hash->capacidad >= 0.75) {
		if (!rehash(hash))
			return false;
	}

	size_t indice = funcion_hash(clave, hash->capacidad);

	lista_t *bucket = hash->tabla[indice];

	par_t clave_buscada;
	clave_buscada.clave = clave;
	clave_buscada.valor = NULL;

	int posicion =
		lista_buscar_posicion(bucket, &clave_buscada, comparar_claves);

	if (posicion >= 0) {
		par_t *par_existente =
			lista_buscar_elemento(bucket, (size_t)posicion);
		if (!par_existente)
			return false;
		if (encontrado)
			*encontrado = par_existente->valor;

		par_existente->valor = valor;
		return true;
	}

	par_t *nuevo_par = malloc(sizeof(par_t));
	if (!nuevo_par)
		return false;

	nuevo_par->clave = malloc(strlen(clave) + 1);
	if (!nuevo_par->clave) {
		free(nuevo_par);
		return false;
	}
	strcpy(nuevo_par->clave, clave);

	nuevo_par->valor = valor;

	if (!lista_agregar(bucket, nuevo_par)) {
		free(nuevo_par->clave);
		free(nuevo_par);
		return false;
	}

	hash->cantidad++;
	if (encontrado)
		*encontrado = NULL;

	return true;
}

void *hash_buscar(hash_t *hash, char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t indice = funcion_hash(clave, hash->capacidad);
	lista_t *bucket = hash->tabla[indice];
	for (size_t i = 0; i < lista_cantidad(bucket); i++) {
		par_t *par = lista_buscar_elemento(bucket, i);
		if (strcmp(par->clave, clave) == 0)
			return par->valor;
	}

	return NULL;
}

bool hash_contiene(hash_t *hash, char *clave)
{
	return hash_buscar(hash, clave) != NULL;
}

void *hash_quitar(hash_t *hash, char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t indice = funcion_hash(clave, hash->capacidad);
	lista_t *bucket = hash->tabla[indice];

	for (size_t i = 0; i < lista_cantidad(bucket); i++) {
		par_t *par = lista_buscar_elemento(bucket, i);
		if (strcmp(par->clave, clave) == 0) {
			par_t *eliminar = lista_eliminar_elemento(bucket, i);
			void *valor = eliminar->valor;
			free(eliminar->clave);
			free(eliminar);
			hash->cantidad--;
			return valor;
		}
	}
	return NULL;
}

size_t hash_iterar(hash_t *hash, bool (*f)(char *, void *, void *), void *ctx)
{
	if (!hash || !f)
		return 0;

	size_t cantidad = 0;

	for (size_t i = 0; i < hash->capacidad; i++) {
		lista_iterador_t *it = lista_iterador_crear(hash->tabla[i]);
		if (!it)
			return cantidad;

		while (lista_iterador_hay_mas_elementos(it)) {
			par_t *par = lista_iterador_obtener_actual(it);
			cantidad++;
			if (!f(par->clave, par->valor, ctx)) {
				lista_iterador_destruir(it);
				return cantidad;
			}

			lista_iterador_siguiente(it);
		}
		lista_iterador_destruir(it);
	}

	return cantidad;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	for (size_t i = 0; i < hash->capacidad; i++) {
		lista_t *bucket = hash->tabla[i];

		while (lista_cantidad(bucket) > 0) {
			par_t *par =
				(par_t *)lista_eliminar_elemento(bucket, 0);
			if (destructor)
				destructor(par->valor);
			free(par->clave);
			free(par);
		}
		lista_destruir(bucket);
	}
	free(hash->tabla);
	free(hash);
}