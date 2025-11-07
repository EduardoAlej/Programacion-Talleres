#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Definiciones.h"

void limpiarBuffer() {
    while (getchar() != "\n");
}
int ingresarP(char nombres[MaxP][MaxN], float precios[MaxP]){
    int cantidadP;
    printf("Cuantos productos quiere ingresar, son maximo %d/n", MaxP);
    scanf("%d", &cantidadP);
    limpiarBuffer();
    if (cantidadP > MaxP){
        printf("No puedes pasarte de %d productos\n", MaxP);
        cantidadP=MaxP;
    }
    for(int i=0; i<cantidadP;i++){
        printf("Producto %d, escriba el nombre del producto \n", i+1);
        fgets(nombres[i], MaxN,stdin);
        nombres[i][strcspn(nombres[i], "\n")]=0;
        printf("Inserte el precio del producto\n");
        scanf("%f", &precios[i]);
        limpiarBuffer();
    }
    return cantidadP;
}

float calcPT(float precios[MaxP], int cantidadP){
    float total= 0;
    for(int i=0;i<cantidadP;i++){
        total += precios[i];
    }
    return total;
}
void MAXMIN(char nombres[MaxP][MaxN], float precios[MaxP], int cantidadP, char caro, char barato){
 if(cantidadP==0){
    printf("No hay productos\n");
    return;
 }
 float caro = precios[0];
 float barato =precios[0];
 strcpy(caro, nombres[0]);
 strcpy(barato, nombres[0]);
 for(int i = 1; i< cantidadP; i++){
if(precios[i]>caro){
    caro=precios[i];
    strcpy(caro,nombres[i]);
}
if(precios[i]<barato){
    barato=precios[i];
    strcpy(barato,nombres[i]);
}
 }
}
float Prepromedio(float precios[MaxP], int cantidadP){
    if (cantidadP == 0) return 0;
    return calcPT(precios, cantidadP)/cantidadP;
}
int buscarPro(char nombres[MaxP][MaxN],float precios[MaxP], int cantidadP, const char busqueda){
    int bien=0;
    for(int i=0;i<cantidadP;i++){
        if(strcmp(nombres[i], busqueda)==0){
            printf("Su producto es %s con un precio de: %.2f dolares\n",nombres[i],precios[i]);
            bien=1;
            break;
        }
    }
    if(!bien){
        printf("No existe ese producto");
    }
}