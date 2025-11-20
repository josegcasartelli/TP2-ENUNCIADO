#include "src/tp1.h"
#include "src/menu.h"
#include "src/aux.h"
#include "src/contexto.h"
#include "src/acciones_menu.h"
#include "src/pokedex.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	contexto_t contexto = contexto_crear();

	if (argc >= 2) {
		tp1_t *tp1 = tp1_leer_archivo(argv[1]);
		if (!tp1) {
			fprintf(stderr, "Error al cargar el archivo '%s'\n",
				argv[1]);

		} else {
			pokedex_t *nueva_pokedex = pokedex_crear();
			if (!nueva_pokedex) {
				fprintf(stderr, "Error al crear la pokedex\n");
				tp1_destruir(tp1);
			} else {
				bool exito = tp1_con_cada_pokemon(
					tp1, copiar_pokemon, nueva_pokedex);
				tp1_destruir(tp1);

				if (!exito) {
					fprintf(stderr,
						"Error al cargar los pokemones en la pokedex\n");
					pokedex_destruir(nueva_pokedex);
				} else {
					if (contexto.pokedex)
						pokedex_destruir(
							contexto.pokedex);
					contexto.pokedex = nueva_pokedex;
					printf("Archivo '%s' cargado correctamente\n",
					       argv[1]);
				}
			}
		}
	}
	menu_t *menu = crear_menu_principal(&contexto);
	if (!menu)
		return 1;

	while (!contexto.salir) {
		mostrar_menu_principal(&contexto, menu);

		char opcion = leer_opcion_menu();
		if (opcion == '\0') {
		} else if (opcion == 'q' || opcion == 'Q') {
			accion_salir(&contexto);
		} else {
			menu_ejecutar(menu, opcion);
		}
	}

	if (contexto.pokedex)
		pokedex_destruir(contexto.pokedex);
	menu_destruir(menu);

	return 0;
}
