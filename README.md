<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TP2

## Repositorio de (Jose Gonzalez Casartelli) - (113633) - (jgcasartelli06@gmail.com)

- Para compilar:

```bash
make
```

- Para ejecutar:

```bash
./pruebas_alumno
./tp2
```

- Para ejecutar con valgrind:
```bash
make valgrind-alumno
make valgrind-tp2
```
---


## TP2 - Menú y juego de memoria pokemon

### ¿Qué hace el programa?

**El programa permite:**

1) Cargar una pokédex desde un archivo CSV
2) Buscar Pokemon
	- Por nombre
	- Por ID
3) Mostrar pokemon
	- Ordenados por nombre
	- Ordenados por ID
4) Jugar un juego de memoria
	- Se eligen 9 pokémon de la pokédex
	- Se generan 18 cartas (dos copias de cada pokemon)
	- Dos jugadores se alternan para descubrir parejas
	- Se lleva puntaje, se muestra un resumen y quien gana
	- Se ve el historial completo de jugadas y las últimas 5 jugadas
5) Cambiar el estilo visual del menú
	- Estilo simple
	- Estilo “caja”
	- Estilo con colores
Todo se maneja desde un menú principal por consola

### Estructuras principales

### *Pokedex (pokedex_t)**

La pokédex es una estructura que combina varios TDA para lograr operaciones eficientes

**Hash**
-  Clave: nombre del pokemon (char*)
-  Valor: puntero a struct pokemon
-  Uso principal: búsqueda rápida por nombre
**ABB ordenado por ID**
- Cada nodo tiene un struct pokemon*
- El criterio del árbol compara por el campo id
- Uso principal: mostrar pokémon ordenados por id y buscar por id
**ABB ordenado por nombre**
- También almacena struct pokemon*
- El criterio del árbol compara por nombre
- Uso principal: mostrar pokémon ordenados alfabeticamente

La pokédex, internamente, no copia el pokemon varias veces.  Todos los TDAs apuntan al mismo struct pokemon.  La destrucción final se hace desde el hash, liberando el pokemon y su nombre.

<div align="center">
<img width="70%" src="img/diagrama_pokedex.png">
</div>


### Menu generico (menu_t)

El menú está diseñado como un TDA genérico y reutilizable

Guarda:

- Un titulo (string)
- Un arreglo de opciones
	- Tecla (ej: ‘C’)
	- Texto descriptivo (ej: “cargar archivo”)
	- Acción: función que se ejecuta cuando se elige esa tecla.
    - Un puntero void* contexto que se pasa a las acciones 

Carateristicas:

- No sabe imprimir “solo”: Se le pasa una función de mostrar (menu_mostrar_opcion_t) que se encarga de dibujar cada opción según el estilo deseado.
- Tiene dos funciones principales:
	- menu_mostrar(menu, callback, extra)
	    Recorre las opciones y llama al callback para cada una
	- menu_ejecutar(menu, tecla)
		Busca la opción con esa tecla y ejecuta su acción


<div align="center">
<img width="70%" src="img/diagrama_menu_t.png">
</div>

### Juego de memoria

El juego usa:

- Un vector de cartas (carta_t), cada una con:
    - Un struct pokemon*
	- Un booleano eliminada para saber si ya se encontró la pareja
- Un vector dinámico de jugadas (jugada_t) para el historial completo
- Una cola de tamaño máximo 5 con las últimas 5 jugadas

<div align="center">
<img width="70%" src="img/diagrama_coleccion_pokemones_t.png">
</div>

<div align="center">
<img width="70%" src="img/diagrama_carta_t.png">
</div>

Cada jugada guarda:
- Jugador
- Las dos opciones elegidas
- Los punteros a los pokemones volteados
- Si fue acierto o fallo

### Flujo general del programa

### 1. Inicio (main/tp2.c)

1. Se crea un contexto (contexto_t), que contiene:
	- pokedex_t* pokedex (inicialmente NULL)
	- Bool salir (inicialmente false)
	- Int estilo_menu (inicialmente 0)
2. Si se pasa un archivo por línea de comandos
	- Se intenta leer el archivo usando tp1_leer_archivo
	- Se crea una pokédex nueva
	- Se copian los pokemon desde la estructura del tp1 a la pokédex
	- Si algo falla, se informa el error y se limpia la memoria
3. Se crea el menú principal con crear_menu_principal(&contexto)
	- Opciones:
		- C: cargar archivo
		- B: buscar pokemon
		- M: mostrar pokemon
		- J: jugar
		- S: jugar con semilla
		- E: cambiar estilo
		- Q: salir

4. Comienza el bucle principal

```c
while (!contexto.salir) {
		mostrar_menu_principal(&contexto, menu);

		char opcion = leer_opcion_menu();
```

- Mostrar_menu_principal elige el estilo según contexto.estilo_menu, y llama a menu_mostrar con el callback correspondiente
- Leer_opcion_menu obtiene la tecla del usuario
- Si es ‘Q’ o ‘q’ se llama a la acción salir
- En otro caso, se llama a menu_ejecutar(menú, opción)

### 2. Flujo de las acciones del menú

a) Cargar archivo
 1. Se pide al usuario el nombre del archivo
 2. Se lee el archivo con las funciones del tp1
 3. Se crea una pokédex nueva
 4. Se copian los pokémon del TP1 a la pokédex
 5. Si ya había una pokédex previa, se destruye antes de reemplazarla
 6. Se muestra un mensaje confirmando la carga

Decision tomada:
Se separa la lectura de archivo TP1 de la estructura final (pokédex).  Así, el TP2 no depende de cómo se parseo el csv, solo de los pokemon ya construidos.

b) Buscar pokemon
 - Primero se verifica si hay pokédex cargada.  Si no la hay, 
muestra un mensaje y se vuelve al menú

- Se entra a un submenú de búsqueda:
	- N: buscar por nombre
	- I: buscar por ID
	- A: volver al menú principal
- Búsqueda por nombre:
	- Se pide un sring
	- Se llama a pokedex_buscar_por_nombre
	- Internamente, esto usa el hash, que busca en tiempo O(1)
	- Se muestra la ficha completa del pokemon si existe sino un mensaje
- Busqueda por ID:
	- Se pide un número como texto y se convierte a entero
	- Se llama a pokedex_buscar_por_id
	- Internamente, esto usa el ABB ordenado por ID, tiempo O(log n)
	- También muestra la ficha o un mensaje de no encontrado

c) Mostrar pokemon

- De nuevo, se verifica si hay pokédex cargada
- Submenu:
	- N: mostrar ordenados por nombre
	- I: mostrar ordenados or ID
	- A: volver
- Mostrar por nombre:
	- Se llama a pokedex_mostrar_por_nombre
	- Internamente hace un recorrido inorden del ABB ordenado por nombre
	- La salida se ve en orden alfabético
- Mostrar por id:
	- Se llama a pokedex_mostrar_por_id
	- Hace un recorrido inorden del ABB ordenado por ID
	- Se ve en orden creciente de ID

Decision tomada:
En vez de ordenar cada vez un vector, se guarda la información en árboles ya estructurados para poder recorrerlos directamente en orden.

d) Cambiar estilo de menú

- La accion_cambiar_estilo modifica contexto->estilo_menu:
	- 0->1->2->0 (va ciclando)
- La próxima vez que se llama a mostrar_menu_principal, el menú se verá distinto:
	- Estilo 0: lista simple
	- Estilo 1: opciones dentro de una “caja”
	- Estilo 2: texto con colores (usando códigos ANSI)

Decision tomada:
El menú en sí es genérico.  Los estilos están “afuera”, en funciones que se pasan como callback, para no mezclar presentación con la lógica del TDA menu_t.

e) Salir

- Accion_salir marca contexto->salir = true
- El while (!contexto.salir) termina en main
- Se destruye la pokedex (si existe) y el menú
- El programa finaliza

### 3. Flujo detallado del juego de memoria

Cuando el usuario elige:
 - J: se llama a juego_jugar(contexto->pokedex) con una semilla basada en la hora
 - S: Se pide una semilla al usuario y se llama a juego_jugar_con_semilla

a) Preparación del mazo

 1. Se recorre la pokedex con pokedex_con_cada, construyendo una colección temporal de punteros a pokemon
 2. Se verifica que haya al menos 9 pokémon (sino el juego no tiene sentido)
 3. Se barajan los pokémon para que las 9 cartas elegidas no sean siempre las mismas
 4. Se crean 18 cartas:
	- Para cada una de las posiciones, se generan 2 cartas que apuntan al mismo pokemon
	- Es decir, las parejas comparten el mismo puntero
 5. Se barajan las 18 cartas para desordenar su posición en el tablero

Decisiones tomadas:
Se usan siempre 9 parejas (18 cartas).  Esto simplifica el diseño porque el tamaño del tablero es constante
Cada carta guarda un puntero, no una copia del pokemon.  Esto evita duplicar memoria y mantiene todo consistente

b) Bucle de turnos

Durante la partida se mantienen:

 - Puntaje[2]: puntos de cada jugador
 - Jugador: índice del jugador actual (0 o 1)
 - Parejas_restantes: empieza en 9
 - Un vector dinámico jugadas para guardar todas las jugadas
 - Una cola para las últimas 5 jugadas, mostradas en pantalla
 - Una bandera salir_juego para permitir cortar la partida con q

En cada turno:
1. Se muestra:
	- El turno actual y el puntaje
	- El tablero (con ?? para cartas boca abajo y el nombre para - las ya eliminadas)
	- Las últimas 5 jugadas usando la cola
2. El jugador elige la primera carta:
	- Se pide una posición
	- El ingreso se valida:
	  - Se lee un string
	  - Si el usuario escribe ‘q’ se marca salir_juego = true
	  - Si no, se verifica que sean solo dígitos, que estén en rango y que la carta no esté eliminada
3. Si no se cancelo el juego, el jugador elige la segunda carta:
	- Mismo proceso de validación
	- No se permite elegir la misma posición que antes
4. Se revelan las dos cartas:
	- Se muestran sus nombres
	- Se compara si apuntan al mismo pokemon
	- Si es acierto:
	  - Se marcan ambas cartas como eliminadas
	  - Se suma un punto al jugador
	  - Parejas_restantes disminuye
	  - El mismo jugador vuelve a jugar
	- Si fallo:
	  - Se pasa al otro jugador
5. Se registra la jugada:
	- Se guarda en el vector dinámico (historial completo)
	- Se encola en la cola de ultimas 5 jugadas
	  - Si la cola tiene 5 elementos, antes se desencola el más viejo
	  - Esto mantiene siempre un máximo de 5 jugadas recientes 

El bucle termina cuando pase cualquiera de las siguientes:
	- Parejas_restantes == 0 (se encontraron todas las parejas)
	- Hubo un error de memoria registrando jugadas
	- El usuario salio con q.

c) Final del juego

- Si el usuario salio con q:
	- Se muestra “Juego cancelado. Volviendo al menú…”
- Si hubo error de memoria:
	- Se informa que se interrumpio por ese motivo.
- Si se terminaron las parejas:
	- Se muestran los puntajes finales
	- Se indica qué jugador ganó o si hubo empate
	- Se imprimen las jugadas separadas por jugador
		- Primero, todas las jugadas del jugador 1 y luego las del 2

Finalmente, se libera toda la memoria usada en el juego:

 - Vector dinámico de jugadas
 - Vector de cartas
 - Colección temporal de pokemon

### Resumen de decisiones importantes

**Separar responsabilidades:**
* TP1: lectura y manejo básico del csv de pokemon
* Pokedex (hash + ABB) estructura de datos para búsquedas y recorridos
* Menú: TDA genérico que no “impone” como se muestra, solo guarda opciones
* Juego: lógica de reglas, turnos, historial y uso de la pokédex

**Evitar duplicacion de datos:**
* Todos los TDAs comparten punteros a struct pokemon, evitando copias innecesarias

**Manejo de memoria y errores:**
* Siempre se revisa si los malloc/realloc fallan
* Se libera la memoria en caso de error


## Respuestas a las preguntas teóricas


### TDAs utilizados


**Hash**

Se utilizó un hash para la búsqueda por nombre en la pokédex.  Esto permite búsquedas en tiempo promedio O(1) y el nombre del pokémon es una clave natural por ser string.  Resulta más adecuado que una lista por ejemplo que tiene búsqueda en tiempo O(n) y a la vez es más rápido que un ABB.


**ABB por ID**

Se usa un ABB donde la clave es el campo id del pokemon.  Esto permite buscar por id en tiempo O(1) y recorrer en orden creciente con un recorrido inorden.  Esto evita tener que ordenar vectores cada vez que se quieran mostrar los pokémons ordenados por id.


**ABB por nombre**

Similar al anterior, pero comparando por nombre.  Se usa exclusivamente para mostrar los pokémon ordenados alfabeticamente.  Es más rápido que las demás estructuras y de nuevo, el recorrido inorden da la salida ordenada sin costos extra.


**Lista + Cola**

La cola se implementa sobre la lista.  Se usa para almacenar las últimas 5 jugadas del juego en memoria, donde la regla FIFO encaja naturalmente con el requisito de "últimas jugadas".


### TDA menu


El TDA menu_t modela un menú genérico, independiente del tp2:
Almacena:
 - un titulo
 - un arreglo de operaciones, cada una con tecla, texto y puntero a función
 - un puntero void* contexto que se pasa a todas las acciones
Operaciones:
 - menu_crear() construye el menu copiando titulo y opciones, y guarda el contexto
 - menu_mostrar() recorre las opciones y llama a un callback de dibujo.  El menú no decide cómo se imprime, eso queda fuera del TDA.
 - menu_ejecutar() busca la opción cuya tecla coincide y ejecuta la acción asociada, devolviendo true o false según haya encontrado o no.
 - menu_destruir() libera todos los recursos asociados


Con este diseño el menú es reutilizable.  No conoce estilos, ni teclas especiales, ni que se trata de pokemon.  Solo administra opciones y delega la presentación y la lógica a otros módulos.


### Otras estructuras auxiliares


pokedex_t: es la estructura "principal" sobre la que trabaja el TP2, pero a la vez funciona como un TDA compuesto, que agrupa otros TDAs:
 - Un hash que usa como claves los nombres de los pokemon
 - Un ABB ordenado por ID, usado para buscar por ID y mostrar por ID en orden creciente
 - Un ABB ordenado por nombre, usado para mostrar ordenado alfabeticamente


La pokédex no copia los pokemon, solo guarda punteros en cada TDA.  Al destruirla, primero destruye los ABB (sin liberar los pokemon) y finalmente destruye el hash con un destructor que libera el struct pokemon y su nombre.  De esa forma hay un único punto de destrucción y no se producen dobles free


carta_t: representa una carta del tablero. Guarda un puntero a struct pokemon y un booleano eliminado.  Las dos cartas de una misma pareja comparten el mismo puntero


jugada_t: representa una jugada de un jugador (número de jugador, posiciones elegidas, punteros a los pokémon levantados y si fue acierto o fallo).  Se guarda en el vector de historial y en la cola de últimas jugadas.


coleccion_pokemones_t: estructura interna para acumular punteros a pokemon al recorrer la pokedex con el iterador interno.  Se usa solo durante la preparación del mazo y luego se libera.


En todos los casos, estas estructuras comparten punteros a struct pokémon con la pokédex, sin duplicar datos, y confían en la destrucción de la pokédex para liberar cada pokemon.


### Dificultades y soluciones

**Mantener el menú genérico**

Inicialmente los estilos y la tecla de cambio de estilo estaban dentro de menu.c, lo que rompía la genericidad.  Se corrigió moviendo los estilos a acciones_menu.c y haciendo que menu_mostrar reciba una función de impresión.  El menú dejó de decidir cómo se ve o que tecla cambia el estilo.


**Compartir pokemon entre hash y ABB**

Había riesgo de duplicar memoria y complicar la destrucción si cada TDA ocupaba la estructura.  Se resolvió haciendo que todos los TDAs almacenen punteros al mismo struct pokemon, y usando una única función destructora al destruir la pokédex.
