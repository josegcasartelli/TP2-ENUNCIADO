#ifndef AUX_H
#define AUX_H

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "tp1.h"

char* leer_linea(FILE* archivo);

void limpiar_buffer(void);

char leer_opcion_menu(void);

int comparar_por_id(const void* a, const void* b);
int comparar_por_nombre(const void* a, const void* b);
bool copiar_pokemon(struct pokemon* original, void* extra);



#endif
