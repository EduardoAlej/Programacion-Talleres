#ifndef CLIENTES_H
#define CLIENTES_H

typedef struct {
    int idc;
    char nombre[100];
    int edad;
    float presup;
} Cliente;

void agregarC();
void mostrarC();
Cliente* buscarCID(int id);
int getLastIDC();

#endif