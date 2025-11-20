#include "acciones_menu.h"
#include "contexto.h"
#include "aux.h"
#include "juego.h"
#include "tp1.h"
#include "pokedex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void accion_cargar(void *ctx)
{
	contexto_t *contexto = ctx;

	printf("Ingrese el nombre del archivo: ");
	char *nombre_archivo = leer_linea(stdin);
	if (!nombre_archivo) {
		printf("Error leyendo el nombre del archivo.\n");
		return;
	}

	tp1_t *tp1 = tp1_leer_archivo(nombre_archivo);
	free(nombre_archivo);

	if (!tp1) {
		printf("Error al cargar el archivo.\n");
		return;
	}

	pokedex_t *nueva_pokedex = pokedex_crear();
	if (!nueva_pokedex) {
		tp1_destruir(tp1);
		printf("Error al crear la pokedex\n");
		return;
	}

	bool exito = tp1_con_cada_pokemon(tp1, copiar_pokemon, nueva_pokedex);

	tp1_destruir(tp1);
	if (!exito) {
		pokedex_destruir(nueva_pokedex);
		printf("Error al cargar los pokemones en la pokedex\n");
		return;
	}

	if (contexto->pokedex)
		pokedex_destruir(contexto->pokedex);

	contexto->pokedex = nueva_pokedex;
	printf("Archivo caragdo correctamente\n");
}

void accion_buscar(void *ctx)
{
	contexto_t *contexto = ctx;

	if (!contexto->pokedex) {
		printf("No hay una pokedex cargada.  Use la opcion 'Cargar archivo' primero.\n");
		return;
	}

	bool salir_submenu = false;

	while (!salir_submenu) {
		printf("\n--- Buscar pokemon ---\n");
		printf("N. Buscar por nombre\n");
		printf("I. Buscar por ID\n");
		printf("A. Volver al menu principal\n");

		char opcion = leer_opcion_menu();

		if (opcion == 'A' || opcion == 'a') {
			salir_submenu = true;
		} else if (opcion == 'N' || opcion == 'n') {
			printf("Ingrese el nombre del pokemon: ");
			char *nombre = leer_linea(stdin);
			if (!nombre) {
				printf("Error leyendo el nombre\n");
			}

			struct pokemon *p = pokedex_buscar_por_nombre(
				contexto->pokedex, nombre);
			if (p) {
				printf("ID=%d, Nombre=%s, Tipo=%d, Ataque=%d, Defensa=%d, Velocidad=%d\n",
				       p->id, p->nombre, p->tipo, p->ataque,
				       p->defensa, p->velocidad);
			} else {
				printf("No se encontro ningun pokemon con nombre '%s'\n",
				       nombre);
			}
			free(nombre);
		} else if (opcion == 'I' || opcion == 'i') {
			printf("Ingrese el ID del pokemon: ");
			char *linea = leer_linea(stdin);
			if (!linea) {
				printf("Error leyendo el ID\n");
			}
			int id = atoi(linea);
			free(linea);

			struct pokemon *p =
				pokedex_buscar_por_id(contexto->pokedex, id);
			if (p) {
				printf("ID=%d, Nombre=%s, Tipo=%d, Ataque=%d, Defensa=%d, Velocidad=%d\n",
				       p->id, p->nombre, p->tipo, p->ataque,
				       p->defensa, p->velocidad);
			} else {
				printf("No se encontro ningun pokemon con ID '%d'\n",
				       id);
			}
		} else {
			printf("Opcion invalida\n");
		}
	}
}

void accion_mostrar(void *ctx)
{
	contexto_t *contexto = ctx;

	if (!contexto->pokedex) {
		printf("No hay una pokedex cargada.  Use la opcion 'cargar archivo' primero\n");
		return;
	}

	bool salir_submenu = false;

	while (!salir_submenu) {
		printf("\n--- Mostrar pokemon ---\n");
		printf("N. Mostrar ordenados por nombre\n");
		printf("I. Mostrar ordenados por ID\n");
		printf("A. Volver al menu principal\n");

		char opcion = leer_opcion_menu();

		if (opcion == 'A' || opcion == 'a') {
			salir_submenu = true;
		} else if (opcion == 'N' || opcion == 'n') {
			printf("\nPokemon ordenados por nombre:\n");
			pokedex_mostrar_por_nombre(contexto->pokedex);
		} else if (opcion == 'I' || opcion == 'i') {
			printf("\nPokemon ordenados por ID:\n");
			pokedex_mostrar_por_id(contexto->pokedex);
		} else {
			printf("Opcion invalida\n");
		}
	}
}

void accion_jugar(void *ctx)
{
	contexto_t *contexto = ctx;

	if (!contexto->pokedex) {
		printf("No hay una pokedex cargada, use la opcion 'Cargar archivo' primero\n");
		return;
	}

	juego_jugar(contexto->pokedex);
}

void accion_jugar_semilla(void *ctx)
{
	contexto_t *contexto = ctx;

	if (!contexto->pokedex) {
		printf("No hay una pokedex cargada, use la opcion 'Cargar archivo' primero\n");
		return;
	}

	printf("Ingrese la semilla: ");
	char *linea = leer_linea(stdin);
	if (!linea) {
		printf("Error leyendo la semilla\n");
		return;
	}

	unsigned int semilla = (unsigned int)atoi(linea);
	free(linea);

	juego_jugar_con_semilla(contexto->pokedex, semilla);
}
void accion_cambiar_estilo(void *ctx)
{
	contexto_t *contexto = ctx;
	if (!contexto) {
		printf("No se puede cambiar estilo\n");
		return;
	}

	contexto->estilo_menu++;
	if (contexto->estilo_menu >= 3)
		contexto->estilo_menu = 0;

	printf("Estilo cambiado (ahora: %d)\n", contexto->estilo_menu);
}

void accion_salir(void *ctx)
{
	contexto_t *contexto = ctx;
	contexto->salir = true;
	printf("Saliendo del juego...\n");
}

menu_t *crear_menu_principal(contexto_t *contexto)
{
	menu_opcion_t opciones[] = {
		{ 'C', "Cargar archivo", accion_cargar },
		{ 'B', "Buscar pokemon", accion_buscar },
		{ 'M', "Mostrar pokemon", accion_mostrar },
		{ 'J', "Jugar", accion_jugar },
		{ 'S', "Jugar con semilla", accion_jugar_semilla },
		{ 'E', "Cambiar estilo", accion_cambiar_estilo },
		{ 'Q', "Salir", accion_salir }

	};

	size_t cantidad = sizeof(opciones) / sizeof(opciones[0]);

	return menu_crear(
		"TP2 - Juego de memoria con pokemon (hecho por Jose Gonzalez Casartelli)",
		opciones, cantidad, contexto);
}
static void mostrar_opcion_estilo_simple(const char *titulo,
					 const menu_opcion_t *opcion,
					 size_t indice, void *extra)
{
	(void)extra;

	if (indice == 0 && titulo) {
		printf("\n%s\n\n", titulo);
	}

	printf("%c. %s\n", opcion->tecla, opcion->texto);
}

static void mostrar_opcion_estilo_caja(const char *titulo,
				       const menu_opcion_t *opcion,
				       size_t indice, void *extra)
{
	(void)extra;

	if (indice == 0 && titulo) {
		printf("\n%s\n\n", titulo);
		printf("------------------------------\n");
	}

	printf("| %c %-24s |\n", opcion->tecla, opcion->texto);
}

static void mostrar_opcion_estilo_color(const char *titulo,
					const menu_opcion_t *opcion,
					size_t indice, void *extra)
{
	(void)extra;

	if (indice == 0 && titulo) {
		printf("\n%s\n\n", titulo);
	}

	printf("\033[1;36m%c\033[0m. %s\n", opcion->tecla, opcion->texto);
}

void mostrar_menu_principal(contexto_t *contexto, menu_t *menu)
{
	if (!menu)
		return;

	int estilo = 0;
	if (contexto)
		estilo = contexto->estilo_menu;

	switch (estilo) {
	case 0:
		menu_mostrar(menu, mostrar_opcion_estilo_simple, contexto);
		break;

	case 1:

		menu_mostrar(menu, mostrar_opcion_estilo_caja, contexto);
		printf("------------------------------\n");
		break;

	case 2:
		menu_mostrar(menu, mostrar_opcion_estilo_color, contexto);
		break;

	default:
		menu_mostrar(menu, mostrar_opcion_estilo_simple, contexto);
		break;
	}

	printf("\n");
}
