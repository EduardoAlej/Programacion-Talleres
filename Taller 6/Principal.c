#include <stdio.h>
#include <string.h>
#include "Definiciones.h"

int main(){
    char nombres[MaxP][MaxN];
    float precios[MaxP];
    int cantidadP = 0;
    char caro[MaxN];
    char barato[MaxN];
    char busqueda[MaxN];
    int opcion;
    
    do{
        printf("Seleccione una opcion:\n1. Agregar Producto\n2. Precio de todo el inventario \n3. Producto Mas Caro y Mas Barato \n4. Precio promedio \n5. Buscar producto \n6. Salir\n");
        scanf("%d", &opcion);
        limpiarBuffer();
        
        switch(opcion){
            case 1:
                cantidadP = ingresarP(nombres, precios);
                break;
            case 2:
                if(cantidadP > 0){
                    float total = calcPT(precios, cantidadP);
                    printf("El precio del inventario es $%.2f\n", total);
                } else {
                    printf("No hay productos insertados\n");
                }
                break;
            case 3:
                if(cantidadP > 0){
                    MAXMIN(nombres, precios, cantidadP, caro, barato);
                    printf("El producto mas caro es: %s\n", caro);
                    printf("El producto mas barato es: %s\n", barato);
                } else {
                    printf("No hay productos insertados\n");
                }
                break;
            case 4:
                if(cantidadP > 0){
                    float promedio = Prepromedio(precios, cantidadP);
                    printf("El precio promedio es $%.2f\n", promedio);
                } else {
                    printf("No hay productos insertados\n");
                }
                break;
            case 5:
                if(cantidadP > 0){
                    printf("Que producto quieres buscar: ");
                    fgets(busqueda, MaxN, stdin);
                    busqueda[strcspn(busqueda, "\n")] = 0;
                    buscarPro(nombres, precios, cantidadP, busqueda);
                } else {
                    printf("No hay productos insertados\n");
                }
                break;
            case 6:
                printf("Terminando programa\n");
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while(opcion != 6);
    
    return 0;
}