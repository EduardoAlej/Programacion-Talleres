#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Definiciones.h"

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int ingresarP(char nombres[MaxP][MaxN], float precios[MaxP]){
    int cantidadP;
    
    printf("Cuantos productos quiere ingresar (maximo %d): ", MaxP);
    scanf("%d", &cantidadP);
    limpiarBuffer();
    
    if (cantidadP > MaxP){
        printf("No puedes pasarte de %d productos. Se limitara a %d.\n", MaxP, MaxP);
        cantidadP = MaxP;
    }
    
    if (cantidadP < 1){
        printf("Debes ingresar al menos 1 producto.\n");
        return 0;
    }
    
    for(int i = 0; i < cantidadP; i++){
        printf("\n--- Producto %d ---\n", i + 1);
        
        printf("Nombre: ");
        fgets(nombres[i], MaxN, stdin);
        nombres[i][strcspn(nombres[i], "\n")] = 0;
        
        printf("Precio: $");
        while(scanf("%f", &precios[i]) != 1 || precios[i] < 0){
            printf("Precio invalido. Ingresa un numero positivo: $");
            limpiarBuffer();
        }
        limpiarBuffer();
    }
    
    printf("\n%d productos ingresados correctamente!\n", cantidadP);
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