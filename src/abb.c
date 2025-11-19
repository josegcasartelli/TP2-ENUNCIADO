#include "abb.h"
#include "abb_estructura_privada.h"

//FUNCIONES AUXILIARES

nodo_t *nodo_crear(void *dato)
{
	nodo_t *nodo = malloc(sizeof(nodo_t));
	if (!nodo)
		return NULL;

	nodo->dato = dato;
	nodo->izq = NULL;
	nodo->der = NULL;

	return nodo;
}

nodo_t *insertar_nodo(nodo_t *raiz, void *dato,
		      int (*comparador)(const void *, const void *),
		      bool *insertado)
{
	if (!raiz) {
		raiz = nodo_crear(dato);
		if (!raiz)
			return NULL;
		*insertado = true;
		return raiz;
	}

	if (comparador(dato, raiz->dato) <= 0) {
		raiz->izq =
			insertar_nodo(raiz->izq, dato, comparador, insertado);
	} else if (comparador(dato, raiz->dato) > 0) {
		raiz->der =
			insertar_nodo(raiz->der, dato, comparador, insertado);
	} else {
		return raiz;
	}

	return raiz;
}

nodo_t *devolver_encontrado(abb_t *abb, void *dato)
{
	nodo_t *actual = abb->raiz;

	while (actual) {
		int cmp = abb->comparador(dato, actual->dato);

		if (cmp == 0)
			return actual;
		else if (cmp < 0) {
			actual = actual->izq;
		} else {
			actual = actual->der;
		}
	}

	return actual;
}

nodo_t *buscar_maximo(nodo_t *nodo)
{
	while (nodo->der != NULL)
		nodo = nodo->der;
	return nodo;
}

nodo_t *eliminar_nodo(nodo_t *nodo, void *dato,
		      int (*comparador)(const void *, const void *),
		      void **dato_eliminado, bool *eliminado)
{
	if (!nodo)
		return NULL;

	if (comparador(dato, nodo->dato) < 0) {
		nodo->izq = eliminar_nodo(nodo->izq, dato, comparador,
					  dato_eliminado, eliminado);
	} else if (comparador(dato, nodo->dato) > 0) {
		nodo->der = eliminar_nodo(nodo->der, dato, comparador,
					  dato_eliminado, eliminado);
	} else {
		if (*dato_eliminado == NULL)
			*dato_eliminado = nodo->dato;

		//caso 1, el nodo no tiene hijos
		if (nodo->izq == NULL && nodo->der == NULL) {
			*eliminado = true;
			free(nodo);
			return NULL;

		}
		//caso 2, el nodo tiene 1 hijo
		else if (nodo->izq == NULL) {
			nodo_t *temp = nodo;
			nodo = nodo->der;
			free(temp);
			*eliminado = true;

		} else if (nodo->der == NULL) {
			nodo_t *temp = nodo;
			nodo = nodo->izq;
			free(temp);
			*eliminado = true;

		}
		//caso 3, el nodo tiene 2 hijos
		else {
			nodo_t *predecesor = buscar_maximo(nodo->izq);
			nodo->dato = predecesor->dato;
			nodo->izq = eliminar_nodo(nodo->izq, predecesor->dato,
						  comparador, dato_eliminado,
						  eliminado);
		}
	}
	return nodo;
}

bool recorrer(nodo_t *nodo, enum abb_recorrido tipo_recorrido,
	      bool (*f)(void *, void *), void *extra, size_t *cantidad)
{
	if (!nodo || !f)
		return true;

	if (tipo_recorrido == ABB_PREORDEN) {
		(*cantidad)++;
		if (!f(nodo->dato, extra))
			return false;
		if (!recorrer(nodo->izq, tipo_recorrido, f, extra, cantidad))
			return false;
		if (!recorrer(nodo->der, tipo_recorrido, f, extra, cantidad))
			return false;
	} else if (tipo_recorrido == ABB_INORDEN) {
		if (!recorrer(nodo->izq, tipo_recorrido, f, extra, cantidad))
			return false;
		(*cantidad)++;
		if (!f(nodo->dato, extra))
			return false;
		if (!recorrer(nodo->der, tipo_recorrido, f, extra, cantidad))
			return false;
	} else if (tipo_recorrido == ABB_POSTORDEN) {
		if (!recorrer(nodo->izq, tipo_recorrido, f, extra, cantidad))
			return false;
		if (!recorrer(nodo->der, tipo_recorrido, f, extra, cantidad))
			return false;
		(*cantidad)++;
		if (!f(nodo->dato, extra))
			return false;
	}

	return true;
}

size_t vectorizar(nodo_t *nodo, enum abb_recorrido tipo_recorrido, size_t cant,
		  void **vector, size_t *i)
{
	if (!nodo || *i >= cant)
		return *i;
	if (tipo_recorrido == ABB_PREORDEN) {
		vector[(*i)++] = nodo->dato;
		vectorizar(nodo->izq, tipo_recorrido, cant, vector, i);
		vectorizar(nodo->der, tipo_recorrido, cant, vector, i);
	} else if (tipo_recorrido == ABB_INORDEN) {
		vectorizar(nodo->izq, tipo_recorrido, cant, vector, i);
		if (*i < cant)
			vector[(*i)++] = nodo->dato;
		vectorizar(nodo->der, tipo_recorrido, cant, vector, i);
	} else if (tipo_recorrido == ABB_POSTORDEN) {
		vectorizar(nodo->izq, tipo_recorrido, cant, vector, i);
		vectorizar(nodo->der, tipo_recorrido, cant, vector, i);
		if (*i < cant) {
			vector[(*i)++] = nodo->dato;
		}
	}

	return *i;
}

void destruir_nodos(nodo_t *nodo)
{
	if (!nodo)
		return;
	destruir_nodos(nodo->izq);
	destruir_nodos(nodo->der);
	free(nodo);
}

void destruir_todo_nodos(nodo_t *nodo, void (*destructor)(void *))
{
	if (!nodo)
		return;
	destruir_todo_nodos(nodo->izq, destructor);
	destruir_todo_nodos(nodo->der, destructor);
	if (destructor)
		destructor(nodo->dato);
	free(nodo);
}

//FUNCIONES DEL ABB.H

abb_t *abb_crear(int (*comparador)(const void *, const void *))
{
	if (!comparador)
		return NULL;

	abb_t *abb = malloc(sizeof(abb_t));
	if (!abb)
		return NULL;

	abb->cantidad = 0;
	abb->raiz = NULL;
	abb->comparador = comparador;

	return abb;
}

bool abb_insertar(abb_t *abb, void *dato)
{
	if (!abb)
		return false;

	bool insertado = false;
	abb->raiz = insertar_nodo(abb->raiz, dato, abb->comparador, &insertado);
	if (insertado)
		abb->cantidad++;

	return insertado;
}

bool abb_existe(abb_t *abb, void *dato)
{
	if (!abb)
		return false;

	return devolver_encontrado(abb, dato) != NULL;
}

void *abb_buscar(abb_t *abb, void *dato)
{
	if (!abb)
		return NULL;

	nodo_t *encontrado = devolver_encontrado(abb, dato);
	if (encontrado)
		return encontrado->dato;

	return NULL;
}

void *abb_eliminar(abb_t *abb, void *dato)
{
	if (!abb)
		return NULL;

	void *dato_eliminado = NULL;
	bool eliminado = false;
	abb->raiz = eliminar_nodo(abb->raiz, dato, abb->comparador,
				  &dato_eliminado, &eliminado);
	if (eliminado)
		abb->cantidad--;

	return dato_eliminado;
}

void *abb_raiz(abb_t *abb)
{
	if (!abb || !abb->raiz)
		return NULL;

	return abb->raiz->dato;
}

size_t abb_cantidad(abb_t *abb)
{
	if (!abb)
		return 0;

	return abb->cantidad;
}

bool abb_esta_vacio(abb_t *abb)
{
	return (!abb || abb->cantidad == 0);
}

size_t abb_con_cada_elemento(abb_t *abb, enum abb_recorrido tipo_recorrido,
			     bool (*f)(void *, void *), void *extra)
{
	if (!abb || !f)
		return 0;

	size_t cantidad = 0;
	recorrer(abb->raiz, tipo_recorrido, f, extra, &cantidad);
	return cantidad;
}

size_t abb_vectorizar(abb_t *abb, enum abb_recorrido tipo_recorrido,
		      size_t cant, void **vector)
{
	if (!abb || !vector || cant == 0)
		return 0;
	size_t i = 0;
	return vectorizar(abb->raiz, tipo_recorrido, cant, vector, &i);
}

void abb_destruir(abb_t *abb)
{
	if (!abb)
		return;
	destruir_nodos(abb->raiz);
	free(abb);
}

void abb_destruir_todo(abb_t *abb, void (*destructor)(void *))
{
	if (!abb)
		return;
	destruir_todo_nodos(abb->raiz, destructor);
	free(abb);
}
