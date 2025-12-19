#include "BIBLIOTECA_H.h"

int main(void) {
    Libro biblioteca[MAX_LIBROS];
    int contadorLibros = 0;
    int opcion;

    printf("=================================\n");
    printf("   SISTEMA DE GESTION DE BIBLIOTECA\n");
    printf("=================================\n");

    for (;;) {
        mostrarMenu();
        printf("\nSeleccione una opcion (1-6): ");

        if (scanf("%d", &opcion) != 1) {
            printf("Error: Ingrese un numero valido.\n");
            limpiarBuffer();
            continue;
        }

        switch (opcion) {
            case 1:
                registrarLibro(biblioteca, &contadorLibros);
                break;
            case 2:
                mostrarLibros(biblioteca, contadorLibros);
                break;
            case 3:
                buscarLibro(biblioteca, contadorLibros);
                break;
            case 4:
                actualizarEstado(biblioteca, contadorLibros);
                break;
            case 5:
                eliminarLibro(biblioteca, &contadorLibros);
                break;
            case 6:
                if (confirmarSalida()) {
                    printf("\nSaliendo del sistema...\n");
                    printf("Gracias por usar el Sistema de Gestion de Biblioteca!\n");
                    return 0;
                }
                break;
            default:
                printf("Opcion no valida. Intente de nuevo.\n");
        }
    }

    return 0;
}

int confirmarSalida(void) {
    char confirmacion;
    printf("\n¿Esta seguro que desea salir? (s/n): ");
    limpiarBuffer();
    scanf("%c", &confirmacion);

    if (confirmacion == 's' || confirmacion == 'S') return 1;
    printf("Continuando en el sistema...\n");
    return 0;
}

void mostrarMenu(void) {
    printf("\n=================================\n");
    printf("          MENU PRINCIPAL\n");
    printf("=================================\n");
    printf("1. Registrar nuevo libro\n");
    printf("2. Mostrar todos los libros\n");
    printf("3. Buscar libro\n");
    printf("4. Prestar/Devolver libro\n");
    printf("5. Eliminar libro\n");
    printf("6. Salir del programa\n");
    printf("=================================\n");
}