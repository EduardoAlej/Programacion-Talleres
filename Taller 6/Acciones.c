#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Definiciones.h"

void limpiarBuffer() {
    while (getchar() != '\n');
}

int ingresarP(char nombres[MaxP][MaxN], float precios[MaxP]){
    int cantidadP;
    printf("Cuantos productos quiere ingresar, son maximo %d: ", MaxP);
    scanf("%d", &cantidadP);
    limpiarBuffer();
    
    if (cantidadP > MaxP){
        printf("No puedes pasarte de %d productos\n", MaxP);
        cantidadP = MaxP;
    }
    
    for(int i = 0; i < cantidadP; i++){
        printf("Producto %d:\n", i + 1);
        printf("  Nombre: ");
        fgets(nombres[i], MaxN, stdin);
        nombres[i][strcspn(nombres[i], "\n")] = 0;
        
        printf("  Precio: $");
        scanf("%f", &precios[i]);
        limpiarBuffer();
    }
    return cantidadP;
}

float calcPT(float precios[MaxP], int cantidadP){
    float total = 0;
    for(int i = 0; i < cantidadP; i++){
        total += precios[i];
    }
    return total;
}

void MAXMIN(char nombres[MaxP][MaxN], float precios[MaxP], int cantidadP, char caro[MaxN], char barato[MaxN]){
    if(cantidadP == 0){
        printf("No hay productos\n");
        return;
    }
    
    float maxPrecio = precios[0];
    float minPrecio = precios[0];
    strcpy(caro, nombres[0]);
    strcpy(barato, nombres[0]);
    
    for(int i = 1; i < cantidadP; i++){
        if(precios[i] > maxPrecio){
            maxPrecio = precios[i];
            strcpy(caro, nombres[i]);
        }
        if(precios[i] < minPrecio){
            minPrecio = precios[i];
            strcpy(barato, nombres[i]);
        }
    }
}

float Prepromedio(float precios[MaxP], int cantidadP){
    if (cantidadP == 0) return 0;
    return calcPT(precios, cantidadP) / cantidadP;
}

void buscarPro(char nombres[MaxP][MaxN], float precios[MaxP], int cantidadP, char busqueda[MaxN]){
    int encontrado = 0;
    for(int i = 0; i < cantidadP; i++){
        if(strcmp(nombres[i], busqueda) == 0){
            printf("Producto encontrado: %s - $%.2f\n", nombres[i], precios[i]);
            encontrado = 1;
            break;
        }
    }
    if(!encontrado){
        printf("Producto '%s' no encontrado\n", busqueda);
    }
}