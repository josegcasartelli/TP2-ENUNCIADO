#include "pokedex.h"
#include "aux.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct pokedex {
    hash_t* hash;
    abb_t* abb_id;
    abb_t* abb_nombre;
};


void destruir_pokemon(void* p) {
    struct pokemon *poke = p;
    if (poke) {
        free(poke->nombre);
        free(poke);
    }
}

pokedex_t* pokedex_crear() {
    pokedex_t* p = malloc(sizeof(pokedex_t));
    if (!p) 
        return NULL;
    p->hash = hash_crear(3);
    p->abb_id = abb_crear(comparar_por_id);
    p->abb_nombre = abb_crear(comparar_por_nombre);

    if (!p->hash || !p->abb_id || !p->abb_nombre) {
        pokedex_destruir(p);
        return NULL;
    }

    return p;
}

bool pokedex_insertar(pokedex_t* p, struct pokemon* poke) {
    if (!p || !poke)
        return false;

    if (!hash_insertar(p->hash, poke->nombre, poke, NULL))
        return false;
    if (!abb_insertar(p->abb_id, poke))
        return false;
    if (!abb_insertar(p->abb_nombre, poke))
        return false;
    return true;
}



bool buscar_id(void* poke, void* extra) {
    struct pokemon* p = poke;
    int* id_buscado = extra;
    return p->id == *id_buscado;
}

struct pokemon* pokedex_buscar_por_nombre(pokedex_t* p, const char* nombre) {
    if (!p || !nombre)
        return NULL;
    return hash_buscar(p->hash, (char*)nombre);
}

struct pokemon* pokedex_buscar_por_id(pokedex_t* p, int id) {
    if (!p)
        return NULL;
    struct pokemon clave = {.id = id};
    return abb_buscar(p->abb_id, &clave);
}

bool mostrar_pokemon(void* dato, void* extra) {
    (void)extra;
    struct pokemon* p  = dato;
    printf("ID=%d, Nombre=%s, Tipo=%d, Ataque=%d, Defensa=%d, Velocidad=%d\n",
p->id, p->nombre, p->tipo, p->ataque, p->defensa, p->velocidad);
return true;
}

void pokedex_mostrar_por_id(pokedex_t* p) {
    if (!p)
        return;
    abb_con_cada_elemento(p->abb_id, ABB_INORDEN, mostrar_pokemon, NULL);
}

void pokedex_mostrar_por_nombre(pokedex_t* p) {
    if (!p)
        return;
    abb_con_cada_elemento(p->abb_nombre, ABB_INORDEN, mostrar_pokemon, NULL);
}


bool wrapper_abb(void* dato, void* extra) {
    struct pokemon* p = dato;
    struct {
        bool (*f)(struct pokemon*, void*);
        void* extra;
        bool ok;
    } *ctx = extra;

    if (!ctx->f(p, ctx->extra)) {
        ctx->ok = false;
        return false;
    }
    return true;
}

size_t pokedex_con_cada(pokedex_t* p,
                        bool (*f)(struct pokemon*, void* extra),
                        void* extra)
{
    if (!p || !f)
        return 0;

    struct {
        bool (*f)(struct pokemon*, void*);
        void* extra;
        bool ok;
    } ctx = { f, extra, true };

    return abb_con_cada_elemento(p->abb_id, ABB_INORDEN,
                                 wrapper_abb, &ctx);
}

void pokedex_destruir(pokedex_t* p) {
    if (!p) return;
    abb_destruir_todo(p->abb_id, NULL);
    abb_destruir_todo(p->abb_nombre, NULL); 
    hash_destruir_todo(p->hash, destruir_pokemon);
    free(p);
}