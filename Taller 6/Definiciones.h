#ifndef DEFINICIONES_H
#define DEFINICIONES_H
#define MaxP 10
#define MaxN 50

void ingresarP(char nombres[MaxP][MaxN], float precios[MaxP]);
float calcPT(float precios[MaxP], int cantidadP);
void MAXMIN(char nombres[MaxP][MaxN], float precios[MaxP], int cantidadP, char caro, char barato);
float Prepromedio(float precios[MaxP], int cantidadP);
int buscarPro(char nombres[MaxP][MaxN], int cantidadP, const char busqueda);
void limpiarBuffer();
#endif 