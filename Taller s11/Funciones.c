#include "BIBLIOTECA_H.h"

int validarID(Libro libros[], int contador, int id) {
    for (int i = 0; i < contador; i++) {
        if (libros[i].id == id) return 1;
    }
    return 0;
}

int esTituloDuplicado(Libro libros[], int contador, char titulo[]) {
    for (int i = 0; i < contador; i++) {
        if (strcmp(libros[i].titulo, titulo) == 0) return 1;
    }
    return 0;
}

void registrarLibro(Libro libros[], int *contador) {
    if (*contador >= MAX_LIBROS) {
        printf("Error: Se alcanzo el maximo de %d libros.\n", MAX_LIBROS);
        return;
    }

    Libro nuevoLibro;
    int idTemp;

    do {
        printf("Ingrese ID del libro: ");
        if (scanf("%d", &idTemp) != 1) {
            printf("Error: ID debe ser numero.\n");
            limpiarBuffer();
            continue;
        }
        if (idTemp <= 0) {
            printf("Error: ID debe ser positivo.\n");
            continue;
        }
        if (validarID(libros, *contador, idTemp)) {
            printf("Error: ID %d ya existe.\n", idTemp);
        } else {
            nuevoLibro.id = idTemp;
            break;
        }
    } while (1);

    limpiarBuffer();

    char tituloTemp[MAX_TITULO];
    printf("Ingrese titulo: ");
    fgets(tituloTemp, MAX_TITULO, stdin);
    tituloTemp[strcspn(tituloTemp, "\n")] = 0;

    if (esTituloDuplicado(libros, *contador, tituloTemp)) {
        printf("\n¡ATENCION! El titulo '%s' ya existe en la biblioteca.\n", tituloTemp);
        char respuesta;
        printf("¿Es una nueva edicion del mismo libro? (s/n): ");
        scanf("%c", &respuesta);
        limpiarBuffer();
        if (respuesta == 's' || respuesta == 'S') {
            char nuevoTitulo[MAX_TITULO];
            snprintf(nuevoTitulo, MAX_TITULO, "%s.v2", tituloTemp);
            strcpy(tituloTemp, nuevoTitulo);
            printf("Se ingreso: %s\n", tituloTemp);
        } else {
            printf("Registro cancelado. No se permiten titulos duplicados.\n");
            return;
        }
    }

    strcpy(nuevoLibro.titulo, tituloTemp);

    printf("Ingrese autor: ");
    fgets(nuevoLibro.autor, MAX_AUTOR, stdin);
    nuevoLibro.autor[strcspn(nuevoLibro.autor, "\n")] = 0;

    printf("Ingrese año de publicacion: ");
    while (scanf("%d", &nuevoLibro.anio) != 1 || nuevoLibro.anio < 0) {
        printf("Error: Año invalido. Ingrese nuevamente: ");
        limpiarBuffer();
    }

    strcpy(nuevoLibro.estado, "Disponible");
    libros[*contador] = nuevoLibro;
    (*contador)++;

    printf("\n¡Libro registrado exitosamente!\n");
    printf("Titulo registrado: %s\n", nuevoLibro.titulo);
}

void mostrarLibros(Libro libros[], int contador) {
    if (contador == 0) {
        printf("\nNo hay libros registrados.\n");
        return;
    }

    printf("\n%-5s %-30s %-20s %-8s %-12s\n",
           "ID", "TITULO", "AUTOR", "AÑO", "ESTADO");
    printf("----------------------------------------------------------------\n");

    for (int i = 0; i < contador; i++) {
        printf("%-5d %-30s %-20s %-8d %-12s\n",
               libros[i].id,
               libros[i].titulo,
               libros[i].autor,
               libros[i].anio,
               libros[i].estado);
    }

    printf("\nTotal: %d libros\n", contador);
}

void buscarLibro(Libro libros[], int contador) {
    if (contador == 0) {
        printf("\nNo hay libros para buscar.\n");
        return;
    }

    int opcion;
    printf("\n--- BUSCAR LIBRO ---\n");
    printf("1. Buscar por ID\n");
    printf("2. Buscar por titulo\n");
    printf("Seleccione opcion: ");

    if (scanf("%d", &opcion) != 1 || (opcion != 1 && opcion != 2)) {
        printf("Opcion invalida.\n");
        limpiarBuffer();
        return;
    }

    limpiarBuffer();

    if (opcion == 1) {
        int idBuscar;
        printf("Ingrese ID a buscar: ");
        scanf("%d", &idBuscar);

        int encontrado = 0;
        for (int i = 0; i < contador; i++) {
            if (libros[i].id == idBuscar) {
                printf("\n--- LIBRO ENCONTRADO ---\n");
                printf("ID: %d\n", libros[i].id);
                printf("Titulo: %s\n", libros[i].titulo);
                printf("Autor: %s\n", libros[i].autor);
                printf("Año: %d\n", libros[i].anio);
                printf("Estado: %s\n", libros[i].estado);
                encontrado = 1;
                break;
            }
        }

        if (!encontrado) printf("\nNo se encontro libro con ID: %d\n", idBuscar);

    } else {
        char tituloBuscar[MAX_TITULO];
        printf("Ingrese titulo a buscar: ");
        fgets(tituloBuscar, MAX_TITULO, stdin);
        tituloBuscar[strcspn(tituloBuscar, "\n")] = 0;

        int disponibles = 0;
        int prestados = 0;
        int encontrados = 0;

        printf("\n--- RESULTADOS DE BUSQUEDA ---\n");

        for (int i = 0; i < contador; i++) {
            char tituloLibro[MAX_TITULO];
            char busqueda[MAX_TITULO];

            strcpy(tituloLibro, libros[i].titulo);
            strcpy(busqueda, tituloBuscar);

            for (int j = 0; tituloLibro[j]; j++) tituloLibro[j] = tolower((unsigned char)tituloLibro[j]);
            for (int j = 0; busqueda[j]; j++) busqueda[j] = tolower((unsigned char)busqueda[j]);

            if (strstr(tituloLibro, busqueda) != NULL) {
                printf("\nLibro %d:\n", encontrados + 1);
                printf("  ID: %d\n", libros[i].id);
                printf("  Titulo: %s\n", libros[i].titulo);
                printf("  Autor: %s\n", libros[i].autor);
                printf("  Año: %d\n", libros[i].anio);
                printf("  Estado: %s\n", libros[i].estado);
                encontrados++;

                if (strcmp(libros[i].estado, "Disponible") == 0) disponibles++;
                else if (strcmp(libros[i].estado, "Prestado") == 0) prestados++;
            }
        }

        if (encontrados > 0) {
            printf("\n--- RESUMEN PARA \"%s\" ---\n", tituloBuscar);
            printf("Total encontrados: %d\n", encontrados);
            printf("Disponibles: %d\n", disponibles);
            printf("Prestados: %d\n", prestados);
        } else {
            printf("\nNo se encontraron libros con titulo: \"%s\"\n", tituloBuscar);
        }
    }
}

void actualizarEstado(Libro libros[], int contador) {
    if (contador == 0) {
        printf("\nNo hay libros para prestar.\n");
        return;
    }

    printf("\n--- PRESTAR/DEVOLVER LIBRO ---\n");

    int idBuscar;
    printf("\nIngrese ID del libro: ");

    if (scanf("%d", &idBuscar) != 1) {
        printf("Error: ID invalido.\n");
        limpiarBuffer();
        return;
    }

    int encontrado = -1;
    for (int i = 0; i < contador; i++) {
        if (libros[i].id == idBuscar) {
            encontrado = i;
            break;
        }
    }

    if (encontrado == -1) {
        printf("\nNo se encontro libro con ID: %d\n", idBuscar);
        return;
    }

    printf("\nLibro encontrado:\n");
    printf("Titulo: %s\n", libros[encontrado].titulo);
    printf("Autor: %s\n", libros[encontrado].autor);
    printf("Estado actual: %s\n", libros[encontrado].estado);

    if (strcmp(libros[encontrado].estado, "Disponible") == 0) {
        char confirmar;
        printf("\n¿Desea prestar este libro? (s/n): ");
        limpiarBuffer();
        scanf("%c", &confirmar);

        if (confirmar == 's' || confirmar == 'S') {
            strcpy(libros[encontrado].estado, "Prestado");
            printf("\n¡Libro prestado exitosamente!\n");
            printf("Estado actualizado: PRESTADO\n");
        } else {
            printf("Operacion cancelada. El libro sigue DISPONIBLE.\n");
        }
    } else if (strcmp(libros[encontrado].estado, "Prestado") == 0) {
        char confirmar;
        printf("\nEste libro ya está PRESTADO.\n");
        printf("¿Desea marcarlo como devuelto? (s/n): ");
        limpiarBuffer();
        scanf("%c", &confirmar);

        if (confirmar == 's' || confirmar == 'S') {
            strcpy(libros[encontrado].estado, "Disponible");
            printf("\n¡Libro devuelto exitosamente!\n");
            printf("Estado actualizado: DISPONIBLE\n");
        } else {
            printf("Operacion cancelada. El libro sigue PRESTADO.\n");
        }
    } else {
        printf("Error: Estado no reconocido.\n");
    }
}

void eliminarLibro(Libro libros[], int *contador) {
    if (*contador == 0) {
        printf("\nNo hay libros para eliminar.\n");
        return;
    }

    int idEliminar;
    printf("Ingrese ID del libro a eliminar: ");

    if (scanf("%d", &idEliminar) != 1) {
        printf("Error: ID invalido.\n");
        limpiarBuffer();
        return;
    }

    int encontrado = -1;
    for (int i = 0; i < *contador; i++) {
        if (libros[i].id == idEliminar) {
            encontrado = i;
            break;
        }
    }

    if (encontrado == -1) {
        printf("\nNo se encontro libro con ID: %d\n", idEliminar);
        return;
    }

    printf("\nLibro a eliminar:\n");
    printf("Titulo: %s\n", libros[encontrado].titulo);
    printf("Autor: %s\n", libros[encontrado].autor);
    printf("Estado: %s\n", libros[encontrado].estado);

    char confirmar;
    printf("\n¿Esta seguro que desea eliminar este libro? (s/n): ");
    limpiarBuffer();
    scanf("%c", &confirmar);

    if (confirmar == 's' || confirmar == 'S') {
        for (int i = encontrado; i < *contador - 1; i++) libros[i] = libros[i + 1];
        (*contador)--;
        printf("¡Libro eliminado exitosamente!\n");
        printf("Libros restantes: %d\n", *contador);
    } else {
        printf("Eliminacion cancelada.\n");
    }
}

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}