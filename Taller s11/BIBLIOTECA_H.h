#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LIBROS 10
#define MAX_TITULO 100
#define MAX_AUTOR 50

typedef struct {
    int id;
    char titulo[MAX_TITULO];
    char autor[MAX_AUTOR];
    int anio;
    char estado[12];
} Libro;

void mostrarMenu(void);
int confirmarSalida(void);

void registrarLibro(Libro libros[], int *contador);
void mostrarLibros(Libro libros[], int contador);
void buscarLibro(Libro libros[], int contador);
void actualizarEstado(Libro libros[], int contador);
void eliminarLibro(Libro libros[], int *contador);

int validarID(Libro libros[], int contador, int id);
int esTituloDuplicado(Libro libros[], int contador, char titulo[]);
void limpiarBuffer(void);

#endif