#ifndef FUNCIONES_EXAMEN_H
#define FUNCIONES_EXAMEN_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h> // ¡IMPORTANTE para offsetof!

// ==================== MACROS GENÉRICAS ====================
#define MAX_STRING 100
#define TRUE 1
#define FALSE 0

// ==================== FUNCIONES BÁSICAS ====================

/*
LIMPIAR BUFFER - Para limpiar entrada después de scanf
USO EN main.c: 
scanf("%d", &num);
limpiarBuffer();
*/
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
LEER STRING SEGURO - Mejor que scanf para strings
USO EN main.c:
char nombre[50];
leerStringSeguro(nombre, 50, "Ingrese nombre: ");
*/
void leerStringSeguro(char* destino, int maxLongitud, const char* mensaje) {
    printf("%s", mensaje);
    fgets(destino, maxLongitud, stdin);
    destino[strcspn(destino, "\n")] = 0; // Elimina el \n
}

// ==================== FUNCIONES DE BÚSQUEDA ====================

/*
BUSCAR POR ID EN CUALQUIER ARREGLO
PARÁMETROS:
- arreglo: tu arreglo de structs
- cantidad: elementos actuales
- idBuscado: ID a buscar
- tamElemento: sizeof(TuStruct)
- offsetID: offsetof(TuStruct, id)

USO EN main.c:
int pos = buscarPorID(estudiantes, cant, 101, 
                     sizeof(Estudiante), offsetof(Estudiante, id));
*/
int buscarPorID(void* arreglo, int cantidad, int idBuscado, 
                size_t tamElemento, int offsetID) {
    char* base = (char*)arreglo;
    for(int i = 0; i < cantidad; i++) {
        int* idActual = (int*)(base + i * tamElemento + offsetID);
        if(*idActual == idBuscado) {
            return i; // Retorna posición
        }
    }
    return -1; // No encontrado
}

/*
BUSCAR POR NOMBRE (case-insensitive)
USO EN main.c:
int pos = buscarPorNombre(productos, cant, "Laptop",
                         sizeof(Producto), offsetof(Producto, nombre));
*/
int buscarPorNombre(void* arreglo, int cantidad, const char* nombreBuscado,
                    size_t tamElemento, int offsetNombre) {
    char* base = (char*)arreglo;
    
    for(int i = 0; i < cantidad; i++) {
        char* nombreActual = (char*)(base + i * tamElemento + offsetNombre);
        
        if(strcasecmp(nombreActual, nombreBuscado) == 0) {
            return i;
        }
    }
    return -1;
}

// ==================== FUNCIONES DE ELIMINACIÓN ====================

/*
ELIMINAR POR ÍNDICE
USO EN main.c:
if(eliminarPorIndice(notas, &cant, 2, sizeof(Nota))) {
    printf("Eliminado\n");
}
*/
int eliminarPorIndice(void* arreglo, int* cantidad, int indice, size_t tamElemento) {
    if(indice < 0 || indice >= *cantidad) {
        return FALSE;
    }
    
    char* base = (char*)arreglo;
    
    // Mover todos los elementos después del índice
    for(int i = indice; i < *cantidad - 1; i++) {
        void* actual = base + i * tamElemento;
        void* siguiente = base + (i + 1) * tamElemento;
        memcpy(actual, siguiente, tamElemento);
    }
    
    (*cantidad)--;
    return TRUE;
}

/*
ELIMINAR POR ID (usa buscarPorID internamente)
USO EN main.c:
if(eliminarPorID(clientes, &cant, 205,
                sizeof(Cliente), offsetof(Cliente, id))) {
    printf("Cliente eliminado\n");
}
*/
int eliminarPorID(void* arreglo, int* cantidad, int idEliminar,
                  size_t tamElemento, int offsetID) {
    int indice = buscarPorID(arreglo, *cantidad, idEliminar, tamElemento, offsetID);
    
    if(indice == -1) {
        return FALSE;
    }
    
    return eliminarPorIndice(arreglo, cantidad, indice, tamElemento);
}

// ==================== FUNCIONES DE ORDENACIÓN ====================

/*
ORDENAR POR CAMPO NUMÉRICO (burbuja)
USO EN main.c:
// Orden ascendente por edad:
ordenarPorCampoNumerico(personas, cant, sizeof(Persona),
                       offsetof(Persona, edad), TRUE);
// Orden descendente por salario:
ordenarPorCampoNumerico(empleados, cant, sizeof(Empleado),
                       offsetof(Empleado, salario), FALSE);
*/
void ordenarPorCampoNumerico(void* arreglo, int cantidad, size_t tamElemento,
                             int offsetCampo, int ascendente) {
    char* base = (char*)arreglo;
    char temp[tamElemento];
    
    for(int i = 0; i < cantidad - 1; i++) {
        for(int j = 0; j < cantidad - i - 1; j++) {
            void* elem1 = base + j * tamElemento;
            void* elem2 = base + (j + 1) * tamElemento;
            
            int* valor1 = (int*)((char*)elem1 + offsetCampo);
            int* valor2 = (int*)((char*)elem2 + offsetCampo);
            
            int intercambiar = ascendente ? (*valor1 > *valor2) : (*valor1 < *valor2);
            
            if(intercambiar) {
                memcpy(temp, elem1, tamElemento);
                memcpy(elem1, elem2, tamElemento);
                memcpy(elem2, temp, tamElemento);
            }
        }
    }
}

/*
ORDENAR POR STRING (alfabéticamente)
USO EN main.c:
ordenarPorCampoString(libros, cant, sizeof(Libro),
                     offsetof(Libro, titulo), TRUE);
*/
void ordenarPorCampoString(void* arreglo, int cantidad, size_t tamElemento,
                           int offsetCampo, int ascendente) {
    char* base = (char*)arreglo;
    char temp[tamElemento];
    
    for(int i = 0; i < cantidad - 1; i++) {
        for(int j = 0; j < cantidad - i - 1; j++) {
            void* elem1 = base + j * tamElemento;
            void* elem2 = base + (j + 1) * tamElemento;
            
            char* texto1 = (char*)elem1 + offsetCampo;
            char* texto2 = (char*)elem2 + offsetCampo;
            
            int comp = strcmp(texto1, texto2);
            int intercambiar = ascendente ? (comp > 0) : (comp < 0);
            
            if(intercambiar) {
                memcpy(temp, elem1, tamElemento);
                memcpy(elem1, elem2, tamElemento);
                memcpy(elem2, temp, tamElemento);
            }
        }
    }
}

// ==================== FUNCIONES DE VALIDACIÓN ====================

/*
VALIDAR SI ID ES ÚNICO
USO EN main.c:
if(!validarIDUnico(arreglo, cant, nuevoID,
                  sizeof(MiStruct), offsetof(MiStruct, id))) {
    // ID es único, proceder
}
*/
int validarIDUnico(void* arreglo, int cantidad, int idVerificar,
                   size_t tamElemento, int offsetID) {
    return buscarPorID(arreglo, cantidad, idVerificar, tamElemento, offsetID) != -1;
}

/*
VALIDAR NÚMERO EN RANGO
USO EN main.c:
int edad = leerNumeroRango("Edad (18-65): ", 18, 65);
*/
int leerNumeroRango(const char* mensaje, int min, int max) {
    int numero;
    do {
        printf("%s", mensaje);
        scanf("%d", &numero);
        limpiarBuffer();
    } while(numero < min || numero > max);
    return numero;
}

// ==================== FUNCIONES DE MATRICES ====================

/*
SUMAR DOS MATRICES
USO EN main.c:
int A[3][3], B[3][3], resultado[3][3];
// ... llenar matrices
sumarMatrices(3, 3, A, B, resultado);
*/
void sumarMatrices(int filas, int columnas, 
                   int A[filas][columnas], 
                   int B[filas][columnas], 
                   int resultado[filas][columnas]) {
    for(int i = 0; i < filas; i++) {
        for(int j = 0; j < columnas; j++) {
            resultado[i][j] = A[i][j] + B[i][j];
        }
    }
}

/*
MOSTRAR MATRIZ
USO EN main.c:
mostrarMatriz(3, 3, matriz);
*/
void mostrarMatriz(int filas, int columnas, int matriz[filas][columnas]) {
    for(int i = 0; i < filas; i++) {
        for(int j = 0; j < columnas; j++) {
            printf("%4d ", matriz[i][j]);
        }
        printf("\n");
    }
}

// ==================== FUNCIONES DE MENÚ ====================

/*
MOSTRAR MENÚ CON OPCIONES
USO EN main.c:
char* opciones[] = {"Registrar", "Mostrar", "Buscar", "Salir"};
mostrarMenu("SISTEMA DE VENTAS", opciones, 4);
*/
void mostrarMenu(const char* titulo, const char* opciones[], int numOpciones) {
    printf("\n=================================\n");
    printf("   %s\n", titulo);
    printf("=================================\n");
    
    for(int i = 0; i < numOpciones; i++) {
        printf("%d. %s\n", i + 1, opciones[i]);
    }
    
    printf("=================================\n");
    printf("Seleccione una opcion: ");
}

#endif // FUNCIONES_EXAMEN_H
/*
===========================================
CÓMO USAR EL HEADER EN TU PROGRAMA PRINCIPAL
===========================================
PASOS:
1. Copia funcionesExamen.h a tu carpeta
2. Inclúyelo en tu main.c
3. Compila incluyendo el header
===========================================
*/

// PASO 1: Incluir el header
#include "funcionesExamen.h"

// PASO 2: Definir tu estructura
typedef struct {
    int id;                // OBLIGATORIO para las funciones
    char nombre[50];
    int edad;
    float promedio;
    // ... otros campos
} Estudiante;

// PASO 3: Declarar variables globales
Estudiante estudiantes[100];
int cantidadEstudiantes = 0;

// PASO 4: Función para agregar (ejemplo)
void agregarEstudiante() {
    if(cantidadEstudiantes >= 100) {
        printf("Capacidad maxima alcanzada\n");
        return;
    }
    
    Estudiante nuevo;
    
    // Usar funciones del header
    do {
        printf("ID: ");
        scanf("%d", &nuevo.id);
        limpiarBuffer(); // ¡NO OLVIDES!
        
        if(validarIDUnico(estudiantes, cantidadEstudiantes, nuevo.id,
                         sizeof(Estudiante), offsetof(Estudiante, id))) {
            printf("ID ya existe. Ingrese otro.\n");
        }
    } while(validarIDUnico(estudiantes, cantidadEstudiantes, nuevo.id,
                          sizeof(Estudiante), offsetof(Estudiante, id)));
    
    // Leer string seguro
    leerStringSeguro(nuevo.nombre, 50, "Nombre: ");
    
    // Leer número con validación
    nuevo.edad = leerNumeroRango("Edad (15-80): ", 15, 80);
    
    estudiantes[cantidadEstudiantes] = nuevo;
    cantidadEstudiantes++;
    printf("Estudiante agregado!\n");
}

// PASO 5: Función para buscar (ejemplo)
void buscarEstudiante() {
    int idBuscado;
    printf("ID a buscar: ");
    scanf("%d", &idBuscado);
    limpiarBuffer();
    
    int pos = buscarPorID(estudiantes, cantidadEstudiantes, idBuscado,
                         sizeof(Estudiante), offsetof(Estudiante, id));
    
    if(pos != -1) {
        printf("Encontrado en posicion %d:\n", pos);
        printf("Nombre: %s, Edad: %d\n", 
               estudiantes[pos].nombre, estudiantes[pos].edad);
    } else {
        printf("No encontrado\n");
    }
}

// PASO 6: Función para eliminar (ejemplo)
void eliminarEstudiante() {
    int idEliminar;
    printf("ID a eliminar: ");
    scanf("%d", &idEliminar);
    limpiarBuffer();
    
    if(eliminarPorID(estudiantes, &cantidadEstudiantes, idEliminar,
                    sizeof(Estudiante), offsetof(Estudiante, id))) {
        printf("Eliminado exitosamente. Quedan %d estudiantes.\n", 
               cantidadEstudiantes);
    } else {
        printf("ID no encontrado\n");
    }
}

// PASO 7: Función para ordenar (ejemplo)
void ordenarEstudiantes() {
    int opcion;
    printf("Ordenar por:\n");
    printf("1. ID (ascendente)\n");
    printf("2. Nombre (A-Z)\n");
    printf("3. Edad (descendente)\n");
    printf("Opcion: ");
    scanf("%d", &opcion);
    limpiarBuffer();
    
    switch(opcion) {
        case 1:
            ordenarPorCampoNumerico(estudiantes, cantidadEstudiantes,
                                   sizeof(Estudiante), 
                                   offsetof(Estudiante, id), TRUE);
            break;
        case 2:
            ordenarPorCampoString(estudiantes, cantidadEstudiantes,
                                 sizeof(Estudiante),
                                 offsetof(Estudiante, nombre), TRUE);
            break;
        case 3:
            ordenarPorCampoNumerico(estudiantes, cantidadEstudiantes,
                                   sizeof(Estudiante),
                                   offsetof(Estudiante, edad), FALSE);
            break;
    }
    printf("Ordenado!\n");
}

// PASO 8: Main con menú
int main() {
    char* opciones[] = {
        "Agregar estudiante",
        "Buscar estudiante",
        "Eliminar estudiante",
        "Ordenar estudiantes",
        "Mostrar todos",
        "Salir"
    };
    
    int opcion;
    
    do {
        mostrarMenu("SISTEMA DE ESTUDIANTES", opciones, 6);
        scanf("%d", &opcion);
        limpiarBuffer();
        
        switch(opcion) {
            case 1: agregarEstudiante(); break;
            case 2: buscarEstudiante(); break;
            case 3: eliminarEstudiante(); break;
            case 4: ordenarEstudiantes(); break;
            case 5: 
                // Mostrar todos
                for(int i = 0; i < cantidadEstudiantes; i++) {
                    printf("%d: %s (%d años)\n", 
                           estudiantes[i].id, 
                           estudiantes[i].nombre, 
                           estudiantes[i].edad);
                }
                break;
            case 6: printf("Saliendo...\n"); break;
            default: printf("Opcion invalida\n");
        }
    } while(opcion != 6);
    
    return 0;
}