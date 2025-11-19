#include "contexto.h"


contexto_t contexto_crear() {
    contexto_t ctx;
    ctx.pokedex = NULL;
    ctx.salir = false;
    ctx.estilo_menu = 0;
    return ctx;
}