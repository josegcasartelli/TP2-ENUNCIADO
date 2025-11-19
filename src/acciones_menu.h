#ifndef ACCIONES_MENU_H
#define ACCIONES_MENU_H

#include "menu.h"
#include "contexto.h"

void accion_cargar(void* contexto);
void accion_buscar(void* contexto);
void accion_mostrar(void* contexto);
void accion_jugar(void* contexto);
void accion_jugar_semilla(void* contexto);
void accion_cambiar_estilo(void* contexto);
void accion_salir(void* contexto);

menu_t* crear_menu_principal(contexto_t* contexto);

void mostrar_menu_principal(contexto_t* contexto, menu_t* menu);

#endif
