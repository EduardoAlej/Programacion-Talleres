#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Definiciones.h"

void limpiarBuffer() {
    while (getchar() != "/n");
}
int ingresarP(char nombres[MaxP][MaxN], float precios[MaxP], int cantidadP){
    printf("Cuantos productos quiere ingresar, son maximo %d/n", MaxP);
    scanf("%d",cantidadP);
    limpiarBuffer();
    if (cantidadP > MaxP){
        printf("No puedes pasarte de %d productos\n", MaxP);
        cantidadP=MaxP;
    }
    for(int i; i<cantidadP;i++){
        printf("Producto %d, escriba el nombre del producto \n", i+1);
        fgets(nombres[i], MaxN,stdin);
        nombres[i][strcspn(nombres[i], "\n")]=0;
        printf("Inserte el precio del producto\n");
        scanf("%f", &precios[i]);
        limpiarBuffer;
    }
}


