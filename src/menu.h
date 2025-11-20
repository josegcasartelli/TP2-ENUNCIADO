#ifndef __MENU_H__
#define __MENU_H__

#include <stddef.h>
#include <stdbool.h>

typedef struct menu menu_t;
typedef void (*menu_accion_t)(void *);

typedef struct {
	char tecla;
	const char *texto;
	menu_accion_t accion;
} menu_opcion_t;

typedef void (*menu_mostrar_opcion_t)(const char *titulo,
				      const menu_opcion_t *opcion,
				      size_t indice, void *extra);

menu_t *menu_crear(const char *titulo, const menu_opcion_t *opciones,
		   size_t cantidad_opciones, void *contexto);

void menu_mostrar(menu_t *menu, menu_mostrar_opcion_t mostrar_opcion,
		  void *extra);

bool menu_ejecutar(menu_t *menu, char tecla);

void menu_destruir(menu_t *menu);

#endif