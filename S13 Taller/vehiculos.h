#ifndef VEHICULOS_H
#define VEHICULOS_H

typedef struct {
    int idv;
    char marca[50];
    char modelo[50];
    int ano;
    float precio;
    char tipo[20];
    int disp;
} Vehiculo;

void agregarV();
void mostrarV();
void buscarVID(int id);
void actDisp(int id, int est);
void buscarVCrit(char* mar, char* tip, float Pmax);
int getLastID();

#endif