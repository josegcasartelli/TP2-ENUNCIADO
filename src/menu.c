#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct menu {
	char *titulo;
	menu_opcion_t *opciones;
	size_t cantidad;
	void *contexto;
};

menu_t *menu_crear(const char *titulo, const menu_opcion_t *opciones,
		   size_t cantidad_opciones, void *contexto)

{
	if (!opciones || cantidad_opciones == 0)
		return NULL;

	menu_t *menu = malloc(sizeof(menu_t));
	if (!menu)
		return NULL;

	menu->titulo = NULL;
	if (titulo) {
		menu->titulo = malloc(strlen(titulo) + 1);
		if (!menu->titulo) {
			free(menu);
			return NULL;
		}
		strcpy(menu->titulo, titulo);
	}

	menu->opciones = malloc(cantidad_opciones * sizeof(menu_opcion_t));
	if (!menu->opciones) {
		free(menu->titulo);
		free(menu);
		return NULL;
	}

	for (size_t i = 0; i < cantidad_opciones; i++)
		menu->opciones[i] = opciones[i];

	menu->cantidad = cantidad_opciones;
	menu->contexto = contexto;

	return menu;
}

void menu_mostrar(menu_t *menu, menu_mostrar_opcion_t mostrar_opcion,
		  void *extra)
{
	if (!menu || !mostrar_opcion)
		return;

	for (size_t i = 0; i < menu->cantidad; i++) {
		mostrar_opcion(menu->titulo, &menu->opciones[i], i, extra);
	}
}

bool menu_ejecutar(menu_t *menu, char tecla)
{
	if (!menu)
		return false;

	char op = (char)toupper((unsigned char)tecla);

	for (size_t i = 0; i < menu->cantidad; i++) {
		char t = (char)toupper((unsigned char)menu->opciones[i].tecla);
		if (t == op) {
			if (menu->opciones[i].accion)
				menu->opciones[i].accion(menu->contexto);
			return true;
		}
	}
	return false;
}

void menu_destruir(menu_t *menu)
{
	if (!menu)
		return;
	free(menu->titulo);
	free(menu->opciones);
	free(menu);
}
