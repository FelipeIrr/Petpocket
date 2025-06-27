# GraphQuest MMO

## Descripción
GraphQuest es un juego tipo MMO desarrollado en lenguaje C que simula un laberinto representado como un grafo, donde el jugador puede recorrer escenarios conectados,  recolectar objetos y administrar un inventario. El programa utiliza los TDA lista enlazada simple y mapa (implementado con una tabla hash) para gestionar el grafo.

##Cómo compilar y ejecutar el código
La siguiente aplicación fue creada en el lenguaje C, por lo  que se puede ejecutar utilizando **Visual Studio Code**, junto con sus respectivas extensiones.

### Requisitos previos:
- Tener instalado Visual Studio Code.
- Instalar la extensión C/C++ (Microsoft) en visual Studio code.
- Tener instalado un compilador de C (como gcc). Si estás en Windows, se recomienda instalar MinGW.
- Tener instalado GIT (para usar git_bash).

### Pasos para compilar y ejecutar:
1. Descargue el archivo .zip y extráigalo en una carpeta de su elección(se recomienda crear una carpeta nueva para extraer los archivos).
2. Abra la carpeta en Visual Studio Code
    .Una vez iniciado Visual Studio Code Selecciona carpeta -> Abrir carpeta, luego selecciona la carpeta donde descomprimiste el archivo .zip.

### Compilar y ejecutar el código
1. acceda al archivo "tarea2". 
2. Abra una nueva terminal: Terminal->Nueva Terminal.
3. Luego acceda a la terminal de git bash.
    .En la parte superior derecha de la terminal puedes ver una flecha hacia abajo, al lado derecho de un signo +, haga click y seleccione Git Bash.
4. En la terminal, debes compilar el programa con el siguiente comando:
    ```bash
   gcc tdas/*.c tarea3.c -o tarea3
5. Una vez compilado, puedes ejecutar la aplicación con:
    ```bash
    ./tarea3

## Funcionalidades del programa
Funcionando correctamente el programa debe:
1. Cargar el juego desde un archivo CSV.
2. Recorrer un grafo que representa escenarios interconectados.
3. Recoger ítems de los escenarios.
4. Descartar ítems de los escenarios.
5. Reiniciar Partida.

## Limitaciones conocidas
- El archivo CSV debe tener un formato válido.
- Solo se puede leer un archivo csv a la vez.

## Posibles mejoras
- Desarrollar una interfaz más amigable para el usuario.
- Implementar enemigos.
- Permitir guardar y cargar partidas.

## Ejemplo de uso
1.  Al inciar el programa:
- Se presenta un menú principal

========================================
            GRAPHQUEST - MENÚ
========================================
1) Cargar Laberinto desde Archivo CSV   
2) Iniciar Partida
3) Salir
Ingrese su opción:

El menú cuenta con tres opciones, la primera para cargar el laberinto desde un archivo csv. La segunda opción incia la partida y la tercera es para salir del programa.

2. Opción dos del menú principal:
- Se presenta un menú de juego con cinco opciones.

--- ¿Qué deseas hacer? ---
1) Recoger ítems
2) Descartar ítems
3) Avanzar en una dirección
4) Reiniciar partida
5) Salir del juego
Elige una opción:

3. Opción 1 y 2 del menú de juego:
- Estas opciones permiten recoger ítems del escenario o descartar ítems del inventario.

Para recoger ítems:
Ingrese el nombre del ítem a recoger (o escriba 'cancelar' para salir):

Para descartar:
Ingrese el nombre del ítem a descartar (o 'cancelar' para salir):

4. Opción 3 del menú de juego:
- Esta opción permite avanzar por los escenarios.

Ingrese dirección (arriba, abajo, izquierda, derecha):

5. Opción 4 del menú de juego:
- Esta opción permite reinciar la partida desde cero (todo el progreso será eliminado).

Se iniciará una nueva partida con los valores predeterminados.
