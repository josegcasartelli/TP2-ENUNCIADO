#include "lista.h"
#include <stdio.h>

typedef struct nodo {
	void *dato;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	nodo_t *primero;
	nodo_t *ultimo;
	size_t cantidad;
};

struct lista_iterador {
	nodo_t *actual;
};

lista_t *lista_crear()
{
	lista_t *lista = malloc(sizeof(lista_t));
	if (!lista)
		return NULL;

	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->cantidad = 0;

	return lista;
}

bool lista_vacia(lista_t *lista)
{
	return (!lista) || (lista->cantidad == 0);
}

size_t lista_cantidad(lista_t *lista)
{
	if (!lista)
		return 0;
	return lista->cantidad;
}

bool lista_agregar(lista_t *lista, void *dato)
{
	if (!lista)
		return false;
	nodo_t *nuevo_nodo = malloc(sizeof(nodo_t));
	if (!nuevo_nodo)
		return false;

	nuevo_nodo->dato = dato;
	nuevo_nodo->siguiente = NULL;

	if (lista_vacia(lista)) {
		lista->primero = nuevo_nodo;
		lista->ultimo = nuevo_nodo;
	} else {
		lista->ultimo->siguiente = nuevo_nodo;
		lista->ultimo = nuevo_nodo;
	}

	lista->cantidad++;
	return true;
}

bool lista_insertar(lista_t *lista, void *elemento, size_t posicion)
{
	if (!lista || lista->cantidad == 0 || posicion > lista->cantidad)
		return false;

	if (posicion == lista->cantidad)
		return lista_agregar(lista, elemento);

	nodo_t *nuevo_nodo = malloc(sizeof(nodo_t));
	if (!nuevo_nodo)
		return false;

	nuevo_nodo->dato = elemento;

	if (posicion == 0) {
		nuevo_nodo->siguiente = lista->primero;
		lista->primero = nuevo_nodo;

	}

	else {
		nodo_t *actual = lista->primero;
		for (size_t i = 0; i < posicion - 1; i++)
			actual = actual->siguiente;

		nuevo_nodo->siguiente = actual->siguiente;
		actual->siguiente = nuevo_nodo;
	}

	lista->cantidad++;
	return true;
}

void *lista_eliminar_elemento(lista_t *lista, size_t posicion)
{
	if (!lista || posicion >= lista->cantidad)
		return NULL;

	nodo_t *nodo_a_eliminar;
	void *dato;

	if (posicion == 0) {
		nodo_a_eliminar = lista->primero;
		dato = nodo_a_eliminar->dato;
		lista->primero = nodo_a_eliminar->siguiente;

		if (lista->cantidad == 1) {
			lista->ultimo = NULL;
		}

	} else {
		nodo_t *actual = lista->primero;
		for (size_t i = 0; i < posicion - 1; i++)
			actual = actual->siguiente;

		nodo_a_eliminar = actual->siguiente;
		dato = nodo_a_eliminar->dato;
		actual->siguiente = nodo_a_eliminar->siguiente;

		if (posicion == lista->cantidad - 1)
			lista->ultimo = actual;
	}

	lista->cantidad--;
	free(nodo_a_eliminar);
	return dato;
}

int lista_buscar_posicion(lista_t *lista, void *elemento,
			  int (*comparador)(const void *, const void *))
{
	if (!lista || !comparador)
		return -1;

	nodo_t *actual = lista->primero;
	int posicion = 0;
	while (actual) {
		if (comparador(actual->dato, elemento) == 0)
			return posicion;
		actual = actual->siguiente;
		posicion++;
	}

	return -1;
}

void *lista_buscar_elemento(lista_t *lista, size_t posicion)
{
	if (!lista || posicion >= lista->cantidad)
		return NULL;

	nodo_t *actual = lista->primero;
	for (size_t i = 0; i < posicion; i++)
		actual = actual->siguiente;

	return actual->dato;
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*f)(void *, void *),
			       void *extra)
{
	if (!lista || !f)
		return 0;

	nodo_t *actual = lista->primero;
	size_t aplicados = 0;

	while (actual) {
		aplicados++;
		if (!f(actual->dato, extra))
			break;
		actual = actual->siguiente;
	}

	return aplicados;
}

void lista_destruir_todo(lista_t *lista, void (*destructor)(void *))
{
	if (!lista)
		return;

	nodo_t *actual = lista->primero;
	while (actual) {
		nodo_t *siguiente = actual->siguiente;
		if (destructor)
			destructor(actual->dato);

		free(actual);
		actual = siguiente;
	}

	free(lista);
}

void lista_destruir(lista_t *lista)
{
	if (!lista)
		return;

	nodo_t *actual = lista->primero;
	while (actual) {
		nodo_t *siguiente = actual->siguiente;
		free(actual);
		actual = siguiente;
	}

	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista)
		return NULL;

	lista_iterador_t *it = malloc(sizeof(lista_iterador_t));
	if (!it)
		return NULL;

	it->actual = lista->primero;

	return it;
}

bool lista_iterador_hay_mas_elementos(lista_iterador_t *it)
{
	return it && it->actual != NULL;
}

void lista_iterador_siguiente(lista_iterador_t *it)
{
	if (!it || !it->actual)
		return;
	it->actual = it->actual->siguiente;
}

void *lista_iterador_obtener_actual(lista_iterador_t *it)
{
	if (!it || !it->actual)
		return NULL;
	return it->actual->dato;
}

void lista_iterador_destruir(lista_iterador_t *it)
{
	free(it);
}
