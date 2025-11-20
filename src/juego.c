#include "juego.h"
#include "aux.h"
#include "cola.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
	struct pokemon **vec;
	size_t cantidad;
	size_t capacidad;
} coleccion_pokemones_t;

typedef struct {
	struct pokemon *pokemon;
	bool eliminada;
} carta_t;

typedef struct {
	int jugador;
	int pos1;
	int pos2;
	struct pokemon *p1;
	struct pokemon *p2;
	bool acierto;
} jugada_t;

static bool agregar_pokemon(struct pokemon *p, void *extra)
{
	coleccion_pokemones_t *col = extra;

	if (col->cantidad == col->capacidad) {
		size_t nueva_cap;
		if (col->capacidad == 0)
			nueva_cap = 16;
		else
			nueva_cap = col->capacidad * 2;
		struct pokemon **nuevo =
			realloc(col->vec, nueva_cap * sizeof(struct pokemon *));
		if (!nuevo)
			return false;

		col->vec = nuevo;
		col->capacidad = nueva_cap;
	}

	col->vec[col->cantidad] = p;
	col->cantidad++;
	return true;
}

static void barajar_pokemones(struct pokemon **vec, size_t n)
{
	if (!vec || n <= 1)
		return;

	for (size_t i = n - 1; i > 0; i--) {
		size_t j = (size_t)(rand() % (int)(i + 1));
		struct pokemon *tmp = vec[i];
		vec[i] = vec[j];
		vec[j] = tmp;
	}
}

static void barajar_cartas(carta_t *cartas, size_t n)
{
	if (!cartas || n <= 1)
		return;

	for (size_t i = n - 1; i > 0; i--) {
		size_t j = (size_t)(rand() % (int)(i + 1));
		carta_t tmp = cartas[i];
		cartas[i] = cartas[j];
		cartas[j] = tmp;
	}
}

static void mostrar_tablero(const carta_t *cartas, size_t cant_cartas)
{
	printf("\nCartas disponibles:\n");

	size_t columnas = 6;
	size_t filas = (cant_cartas + columnas - 1) / columnas;

	for (size_t f = 0; f < filas; f++) {
		for (size_t c = 0; c < columnas; c++) {
			size_t i = f * columnas + c;
			if (i >= cant_cartas)
				break;

			const carta_t *carta = &cartas[i];

			if (carta->eliminada) {
				printf("[%2zu] %-10s ", i,
				       carta->pokemon->nombre);
			} else {
				printf("[%2zu] %-10s ", i, "??");
			}
		}
		printf("\n");
	}
}

static void mostrar_ultimas_jugadas(cola_t *ultimas)
{
	printf("\nUltimas jugadas:\n");

	if (!ultimas || cola_cantidad(ultimas) == 0) {
		printf("ninguna\n");
		return;
	}

	jugada_t *buffer[5];
	size_t n = 0;

	while (cola_cantidad(ultimas) > 0 && n < 5) {
		jugada_t *j = cola_desencolar(ultimas);
		buffer[n] = j;
		n++;
	}

	for (size_t i = 0; i < n; i++) {
		jugada_t *j = buffer[i];
		int num_jugador = j->jugador + 1;
		const char *nombre1;
		if (j->p1)
			nombre1 = j->p1->nombre;
		else
			nombre1 = "?";

		const char *nombre2;
		if (j->p2)
			nombre2 = j->p2->nombre;
		else
			nombre2 = "?";

		const char *resultado;
		if (j->acierto)
			resultado = "ACIERTO";
		else
			resultado = "FALLO";

		printf("J%d: (%d, %s) - (%d, %s) -> %s\n", num_jugador, j->pos1,
		       nombre1, j->pos2, nombre2, resultado);
	}

	for (size_t i = 0; i < n; i++) {
		cola_encolar(ultimas, buffer[i]);
	}
}

static int leer_posicion_valida(const carta_t *cartas, size_t cant_cartas,
				bool *salir_juego)
{
	int pos = -1;
	bool posicion_valida = false;

	while (!posicion_valida && !(*salir_juego)) {
		printf("Ingrese una posicion de carta (0-%zu) o 'q' para salir: ",
		       cant_cartas - 1);

		char *linea = leer_linea(stdin);
		if (!linea) {
			printf("Error leyendo la posicion\n");
		} else {
			bool es_numero = false;

			if ((linea[0] == 'q' || linea[0] == 'Q') &&
			    linea[1] == '\0') {
				*salir_juego = true;
			} else {
				es_numero = true;
				for (int i = 0; linea[i] != '\0'; i++) {
					if (!isdigit((unsigned char)linea[i]))
						es_numero = false;
				}
			}

			if (!es_numero) {
				printf("Ingrese una posicion valida (solo numeros)\n");
			} else {
				pos = atoi(linea);

				if (pos < 0 || (size_t)pos >= cant_cartas) {
					printf("Posicion fuera de rango\n");
				} else if (cartas[pos].eliminada) {
					printf("Esa carta ya fue eliminada\n");
				} else {
					posicion_valida = true;
				}
			}
		}
		free(linea);
	}

	return pos;
}

void juego_jugar_con_semilla(pokedex_t *pokedex, unsigned int semilla)
{
	if (!pokedex) {
		printf("No hay pokedex cargada\n");
		return;
	}

	srand(semilla);

	coleccion_pokemones_t col;
	col.vec = NULL;
	col.cantidad = 0;
	col.capacidad = 0;

	size_t visitados = pokedex_con_cada(pokedex, agregar_pokemon, &col);
	(void)visitados;

	if (col.cantidad < 9) {
		free(col.vec);
		printf("Se necesitan al menos 9 pokemones para jugar (hay %zu)\n",
		       col.cantidad);
		return;
	}

	barajar_pokemones(col.vec, col.cantidad);
	size_t cant_parejas = 9;
	size_t cant_cartas = cant_parejas * 2;

	carta_t *cartas = malloc(cant_cartas * sizeof(carta_t));
	if (!cartas) {
		free(col.vec);
		printf("Error de memoria creamdo cartas\n");
		return;
	}

	for (size_t i = 0; i < cant_parejas; i++) {
		struct pokemon *p = col.vec[i];
		cartas[2 * i].pokemon = p;
		cartas[2 * i].eliminada = false;

		cartas[2 * i + 1].pokemon = p;
		cartas[2 * i + 1].eliminada = false;
	}

	free(col.vec);

	barajar_cartas(cartas, cant_cartas);

	int puntaje[2] = { 0, 0 };
	int jugador = 0;
	size_t parejas_restantes = cant_parejas;

	jugada_t *jugadas = NULL;
	size_t cant_jugadas = 0;
	size_t cap_jugadas = 0;

	bool error_memoria_jugadas = false;
	bool salir_juego = false;
	cola_t *ultimas = cola_crear();
	if (!ultimas)
		printf("No se pudo crear la cola de ultimas jugadas\n");

	while (parejas_restantes > 0 && !error_memoria_jugadas &&
	       !salir_juego) {
		printf("\n======================================\n");
		printf("Turno del jugador %d\n", jugador + 1);
		printf("Puntaje -> J1: %d | J2: %d\n", puntaje[0], puntaje[1]);

		mostrar_tablero(cartas, cant_cartas);
		if (ultimas)
			mostrar_ultimas_jugadas(ultimas);
		else
			printf("\nUltimas jugadas:\n no disponibles\n");

		int pos1 =
			leer_posicion_valida(cartas, cant_cartas, &salir_juego);

		if (!salir_juego) {
			int pos2 = -1;
			bool pos2_valida = false;

			while (!pos2_valida && !salir_juego) {
				pos2 = leer_posicion_valida(cartas, cant_cartas,
							    &salir_juego);
				if (!salir_juego) {
					if (pos2 == pos1) {
						printf("Debe elegir una carta distinta a la primera\n");
					} else {
						pos2_valida = true;
					}
				}
			}

			if (!salir_juego) {
				struct pokemon *p1 = cartas[pos1].pokemon;
				struct pokemon *p2 = cartas[pos2].pokemon;

				printf("\nCartas elegidas:\n");
				printf("Pos %d -> %s\n", pos1, p1->nombre);
				printf("Pos %d -> %s\n", pos2, p2->nombre);

				bool acierto = (p1 == p2);

				if (cant_jugadas == cap_jugadas) {
					size_t nueva_cap;
					if (cap_jugadas == 0)
						nueva_cap = 16;
					else
						nueva_cap = cap_jugadas * 2;

					jugada_t *nuevo = realloc(
						jugadas,
						nueva_cap * sizeof(jugada_t));
					if (!nuevo) {
						printf("Error de memoria registrando jugadas\n");
						error_memoria_jugadas = true;
					} else {
						jugadas = nuevo;
						cap_jugadas = nueva_cap;
					}
				}

				if (!error_memoria_jugadas) {
					jugada_t nueva;
					nueva.jugador = jugador;
					nueva.pos1 = pos1;
					nueva.pos2 = pos2;
					nueva.p1 = p1;
					nueva.p2 = p2;
					nueva.acierto = acierto;

					jugadas[cant_jugadas] = nueva;
					cant_jugadas++;

					if (ultimas) {
						jugada_t *copia = malloc(
							sizeof(jugada_t));
						if (copia) {
							*copia = nueva;

							if (cola_cantidad(
								    ultimas) ==
							    5) {
								jugada_t *vieja = cola_desencolar(
									ultimas);
								free(vieja);
							}

							if (!cola_encolar(
								    ultimas,
								    copia)) {
								free(copia);
							}
						}
					}

					if (acierto) {
						printf("ACIERTO! El jugador %d gana un punto y vuelve a jugar\n",
						       jugador + 1);
						cartas[pos1].eliminada = true;
						cartas[pos2].eliminada = true;
						puntaje[jugador]++;
						parejas_restantes--;
					} else {
						printf("FALLO. Pasa el turno al otro jugador\n");
						jugador = 1 - jugador;
					}
				}
			}
		}
	}

	if (salir_juego) {
		printf("Juego cancelado. Volviendo al menu...\n");
	} else if (error_memoria_jugadas) {
		printf("Juego interrumpido por error de memoria al registrar jugadas.\n");
	} else {
		printf("\n======================================\n");
		printf("Fin del juego!\n");
		printf("Puntaje jugador 1: %d\n", puntaje[0]);
		printf("Puntaje jugador 2: %d\n", puntaje[1]);

		if (puntaje[0] > puntaje[1]) {
			printf("Gana el jugador 1!\n");
		} else if (puntaje[1] > puntaje[0]) {
			printf("Gana el jugador 2!\n");
		} else {
			printf("Empate entre ambos jugadores!\n");
		}

		printf("\nJugadas del jugador 1:\n");
		bool alguna_jugada = false;
		for (size_t i = 0; i < cant_jugadas; i++) {
			if (jugadas[i].jugador == 0) {
				alguna_jugada = true;

				const char *n1;
				if (jugadas[i].p1)
					n1 = jugadas[i].p1->nombre;
				else
					n1 = "?";

				const char *n2;
				if (jugadas[i].p2)
					n2 = jugadas[i].p2->nombre;
				else
					n2 = "?";

				const char *resultado;
				if (jugadas[i].acierto)
					resultado = "ACIERTO";
				else
					resultado = "FALLO";

				printf("(%d, %s) - (%d, %s) -> %s\n",
				       jugadas[i].pos1, n1, jugadas[i].pos2, n2,
				       resultado);
			}
		}
		if (!alguna_jugada)
			printf("ninguna\n");

		printf("\nJugadas del jugador 2:\n");
		alguna_jugada = false;
		for (size_t i = 0; i < cant_jugadas; i++) {
			if (jugadas[i].jugador == 1) {
				alguna_jugada = true;

				const char *n1;
				if (jugadas[i].p1)
					n1 = jugadas[i].p1->nombre;
				else
					n1 = "?";

				const char *n2;
				if (jugadas[i].p2)
					n2 = jugadas[i].p2->nombre;
				else
					n2 = "?";

				const char *resultado;
				if (jugadas[i].acierto)
					resultado = "ACIERTO";
				else
					resultado = "FALLO";

				printf("(%d, %s) - (%d, %s) -> %s\n",
				       jugadas[i].pos1, n1, jugadas[i].pos2, n2,
				       resultado);
			}
		}
		if (!alguna_jugada)
			printf("ninguna\n");
	}

	if (ultimas) {
		while (cola_cantidad(ultimas) > 0) {
			jugada_t *j = cola_desencolar(ultimas);
			free(j);
		}
		cola_destruir(ultimas);
	}

	free(jugadas);
	free(cartas);
}

void juego_jugar(pokedex_t *pokedex)
{
	unsigned int semilla = (unsigned int)time(NULL);
	juego_jugar_con_semilla(pokedex, semilla);
}
