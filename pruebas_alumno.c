#include "pa2m.h"
#include "src/pokedex.h"
#include "src/menu.h"
#include <stdlib.h>
#include <string.h>



struct pokemon* pokemon_crear_para_test(int id, const char* nombre)
{
	struct pokemon* p = malloc(sizeof(struct pokemon));
	if (!p)
		return NULL;

	p->id = id;

	p->nombre = malloc(strlen(nombre) + 1);
	if (!p->nombre) {
        free(p);
        return NULL;
    }
    strcpy(p->nombre, nombre);

	p->tipo = 0;
    p->ataque = 0;
    p->defensa = 0;
    p->velocidad = 0;

    return p;

}

void pokemon_destruir_test(struct pokemon *p)
{
	if (!p)
		return;
	free(p->nombre);
	free(p);
}

pokedex_t* pokedex_con_3_pokemones()
{
	pokedex_t* pokedex = pokedex_crear();
	if (!pokedex)
		return NULL;
	
	struct pokemon* Pikachu = pokemon_crear_para_test(1, "Pikachu");
	struct pokemon* Charmander = pokemon_crear_para_test(2, "Charmander");
	struct pokemon* Bulbasaur = pokemon_crear_para_test(3, "Bulbasaur");


	if (!Bulbasaur || !Charmander || ! Pikachu) {
		pokemon_destruir_test(Pikachu);
		pokemon_destruir_test(Charmander);
		pokemon_destruir_test(Bulbasaur);

		pokedex_destruir(pokedex);
		return NULL;
		
	}

	if (!pokedex_insertar(pokedex, Pikachu) || !pokedex_insertar(pokedex, Charmander) ||
		!pokedex_insertar(pokedex, Bulbasaur)) {
		pokedex_destruir(pokedex);
		return NULL;
		}
	

	return pokedex;
}

void pruebas_pokedex_NULL()
{
	pokedex_t* pokedex = NULL;

	pa2m_afirmar(pokedex_buscar_por_nombre(pokedex, "Pikachu") == NULL, "buscar por nombre en pokedex NULL devuelve NULL");
	pa2m_afirmar(pokedex_buscar_por_id(pokedex, 1) == NULL, "buscar por id en pokedex NULL devuelve NULL");

	pokedex = pokedex_crear();
	pa2m_afirmar(pokedex != NULL, "Se puede crear una pokedex vacia");

	pa2m_afirmar(pokedex_buscar_por_nombre(pokedex, NULL) == NULL, "Buscar por nombre con nombre NULL devuelve NULL");
	pokedex_mostrar_por_nombre(NULL);
	pokedex_mostrar_por_id(NULL);
	pa2m_afirmar(true, "Mostrar por nombre/id con pokedex NULL no trae errores");
	pokedex_destruir(pokedex);

}	


void pruebas_pokedex_insertar()
{
	pokedex_t* pokedex = pokedex_crear();
	pa2m_afirmar(pokedex != NULL, "Se puede crear una pokedex vacia para probar inserciones");

	struct pokemon* p1 = pokemon_crear_para_test(1, "poke1");
	pa2m_afirmar(p1 != NULL, "Se crea un pokemon de prueba");

	pa2m_afirmar(pokedex_insertar(pokedex, p1), "Insertar pokemon valido en pokedex vacia devuelve true");

	struct pokemon* buscado = pokedex_buscar_por_id(pokedex, 1);
	pa2m_afirmar(buscado == p1, "Se encontro el pokemon insertado por id");

	buscado = pokedex_buscar_por_nombre(pokedex, "poke1");
	pa2m_afirmar(buscado == p1, "Se encontro el pokemon insertado por nombre");

	struct pokemon* p2 = pokemon_crear_para_test(2, "poke2");
	pa2m_afirmar(p2 != NULL, "Se crea un pokemon de prueba");

	pa2m_afirmar(pokedex_insertar(pokedex, p2), "Insertar pokemon valido en pokedex ya no vacia devuelve true");

	buscado = pokedex_buscar_por_id(pokedex, 2);
	pa2m_afirmar(buscado == p2, "Se encontro el pokemon insertado por id");

	buscado = pokedex_buscar_por_nombre(pokedex, "poke2");
	pa2m_afirmar(buscado == p2, "Se encontro el pokemon insertado por nombre");

	pokedex_destruir(pokedex);


}

void pruebas_busqueda_con_pokedex()
{
	pokedex_t* pokedex = pokedex_con_3_pokemones();
	pa2m_afirmar(pokedex != NULL, "Se crea una pokedex con 3 pokemones de prueba");

	struct pokemon* buscado = pokedex_buscar_por_nombre(pokedex, "Pikachu");
	pa2m_afirmar(buscado->id == 1 && strcmp(buscado->nombre, "Pikachu") == 0, "Buscar por nombre devuelve pokemon correcto");

	buscado = pokedex_buscar_por_nombre(pokedex, "Charmander");
	pa2m_afirmar(buscado->id == 2 && strcmp(buscado->nombre, "Charmander") == 0, "Buscar por nombre devuelve pokemon correcto");

	buscado = pokedex_buscar_por_nombre(pokedex, "Bulbasaur");
	pa2m_afirmar(buscado->id == 3 && strcmp(buscado->nombre, "Bulbasaur") == 0, "Buscar por nombre devuelve pokemon correcto");

	buscado = pokedex_buscar_por_id(pokedex, 1);
	pa2m_afirmar(strcmp(buscado->nombre, "Pikachu") == 0, "Buscar por id devuelve pokemon correcto");

	buscado = pokedex_buscar_por_id(pokedex, 2);
	pa2m_afirmar(strcmp(buscado->nombre, "Charmander") == 0, "Buscar por id devuelve pokemon correcto");

	buscado = pokedex_buscar_por_id(pokedex, 3);
	pa2m_afirmar(strcmp(buscado->nombre, "Bulbasaur") == 0, "Buscar por id devuelve pokemon correcto");

	pokedex_destruir(pokedex);

}

void pruebas_pokedex_busqueda_inexistente()
{
	pokedex_t* pokedex = pokedex_con_3_pokemones();
	pa2m_afirmar(pokedex != NULL, "Se crea una pokedex con 3 pokemones de prueba");

	struct pokemon* buscado = pokedex_buscar_por_nombre(pokedex, "poke inexistente");
	pa2m_afirmar(buscado == NULL, "Buscar por nombre inexistente devuelve NULL");

	buscado = pokedex_buscar_por_id(pokedex, 90);
	pa2m_afirmar(buscado == NULL, "Buscar por id inexistente devuelve NULL");

	pokedex_destruir(pokedex);
}


void pruebas_pokedex_mostrar()
{
	pokedex_t* pokedex = pokedex_con_3_pokemones();
	pa2m_afirmar(pokedex != NULL, "Se crea una pokedex con 3 pokemones de prueba");

	printf("\n--- mostrar por nombre ---\n");
	pokedex_mostrar_por_nombre(pokedex);

	printf("\n--- mostrar por id ---\n");
	pokedex_mostrar_por_id(pokedex);
	pa2m_afirmar(true, "Mostrar por nombre e id no genera errores en pokedex no vacia");

	pokedex_destruir(pokedex);

}	

typedef struct {
	size_t contador;
} contexto_contador_t;

bool contar_todos(struct pokemon* p, void* extra)
{
	(void)p;
	contexto_contador_t* contexto = extra;
	contexto->contador++;
	return true;
}

typedef struct {
	size_t contador;
	size_t limite;
} contexto_hasta_limite_t;

bool contar_hasta_limite(struct pokemon* p, void* extra)
{
	(void)p;
	contexto_hasta_limite_t* contexto = extra;
    contexto->contador++;
    if (contexto->contador >= contexto->limite)
        return false;
    return true;
}

void pruebas_pokedex_con_cada()
{
	pokedex_t* pokedex = pokedex_con_3_pokemones();
	pa2m_afirmar(pokedex != NULL, "Se crea una pokedex con 3 pokemones de prueba");

	contexto_contador_t ctx_todos = {0};
    size_t aplicados = pokedex_con_cada(pokedex, contar_todos, &ctx_todos);

	pa2m_afirmar(aplicados == 3 && ctx_todos.contador == 3, "pokedex_con_cada recorre todos los pokemones");

	contexto_hasta_limite_t ctx_limite = {0, 2};
    aplicados = pokedex_con_cada(pokedex, contar_hasta_limite, &ctx_limite);

	pa2m_afirmar(aplicados == 2 && ctx_limite.contador == 2, "pokedex_con_cada recorre hasta el limite dado");

	pokedex_destruir(pokedex);


}

typedef struct {
	int veces_llamada;
} contexto_menu_t;

void accion_sumar_llamada(void* contexto)
{
	contexto_menu_t* ctx = contexto;
	if (ctx)
		ctx->veces_llamada++;
}

typedef struct {
	size_t cantidad_llamadas;
	char ultima_tecla;
} contexto_mostrar_t;

void mostrar_opcion_prueba(const char* titulo, const menu_opcion_t* opcion, size_t indice, void* extra)
{
	(void)titulo;
	(void)indice;

	contexto_mostrar_t* contexto = extra;
	contexto->cantidad_llamadas++;
	contexto->ultima_tecla = opcion->tecla;
}

void pruebas_menu()
{
	
	menu_t* menu = menu_crear("Titulo", NULL, 1, NULL);
	pa2m_afirmar(menu == NULL, "menu creado con opciones NULL devuelve NULL");

	menu_opcion_t opcion;
	opcion.tecla = 'A';
	opcion.texto = "Opcion A";
	opcion.accion = accion_sumar_llamada;

	menu = menu_crear("Titulo", &opcion, 0, NULL);
	pa2m_afirmar(menu == NULL, "menu crear con cantidad 0 devuelve NULL");

	contexto_menu_t ctx_menu = {0};
	menu_opcion_t opciones[] = {
        {'A', "Opcion A", accion_sumar_llamada},
        {'B', "Opcion B", accion_sumar_llamada},
    };

	menu = menu_crear("Titulo", opciones, 2, &ctx_menu);
	pa2m_afirmar(menu != NULL, "menu crear con opciones validad devuelve un menu no NULL");

	contexto_mostrar_t ctx_mostrar = {0, '\0'};
    menu_mostrar(menu, mostrar_opcion_prueba, &ctx_mostrar);
    pa2m_afirmar(ctx_mostrar.cantidad_llamadas == 2,
                 "menu_mostrar llama al callback una vez por cada opcion");
    pa2m_afirmar(ctx_mostrar.ultima_tecla == 'B',
                 "El callback ve la ultima tecla correcta (B)");

    pa2m_afirmar(menu_ejecutar(menu, 'a') == true,
                 "menu_ejecutar encuentra la opcion aunque la tecla sea minuscula");
    pa2m_afirmar(ctx_menu.veces_llamada == 1,
                 "Al ejecutar 'a' se llama la accion exactamente una vez");

    pa2m_afirmar(menu_ejecutar(menu, 'B') == true,
                 "menu_ejecutar encuentra la opción con tecla mayuscula");
    pa2m_afirmar(ctx_menu.veces_llamada == 2,
                 "Al ejecutar 'B' se vuelve a llamar la accion");

    pa2m_afirmar(menu_ejecutar(menu, 'X') == false,
                 "menu_ejecutar con tecla inexistente devuelve false");
    pa2m_afirmar(ctx_menu.veces_llamada == 2,
                 "Con tecla inexistente no se llama la accion");

    pa2m_afirmar(menu_ejecutar(NULL, 'A') == false,
                 "menu_ejecutar con menu NULL devuelve false");

    menu_mostrar(NULL, mostrar_opcion_prueba, &ctx_mostrar);
    pa2m_afirmar(true,
                 "menu_mostrar con menu NULL no genera errores (callback no se llama)");


	menu_destruir(menu);

}


int main()
{
	pa2m_nuevo_grupo("Pokedex - casos NULL");
    pruebas_pokedex_NULL();

    pa2m_nuevo_grupo("Pokedex - insertar");
    pruebas_pokedex_insertar();

    pa2m_nuevo_grupo("Pokedex - busqueda existente");
    pruebas_busqueda_con_pokedex();

    pa2m_nuevo_grupo("Pokedex - busqueda inexistente");
    pruebas_pokedex_busqueda_inexistente();

    pa2m_nuevo_grupo("Pokedex - mostrar");
    pruebas_pokedex_mostrar();

    pa2m_nuevo_grupo("Pokedex - con_cada(iterador interno)");
    pruebas_pokedex_con_cada();

    pa2m_nuevo_grupo("Menu");
    pruebas_menu();

	return pa2m_mostrar_reporte();
}
