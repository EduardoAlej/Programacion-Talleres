#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehiculos.h"
#include "clientes.h"
#include "ventas.h"

void mostrarMenu();
void buscarVPers();

int main() {
    int op;
    
    printf("========================================\n");
    printf("  SISTEMA DE GESTION\n");
    printf("  Ruedas de Oro\n");
    printf("========================================\n");
    
    do {
        mostrarMenu();
        printf("\nOpcion: ");
        scanf("%d", &op);
        getchar();
        
        switch(op) {
            case 1: agregarV(); break;
            case 2: mostrarV(); break;
            case 3: agregarC(); break;
            case 4: mostrarC(); break;
            case 5: buscarVPers(); break;
            case 6: regVenta(); break;
            case 7: mostrarVent(); break;
            case 8: printf("\nTotal: $%.2f\n", calcTotal()); break;
            case 9: printf("\nSaliendo...\n"); break;
            default: printf("\nNo valido.\n");
        }
        
        printf("\nEnter para seguir...");
        getchar();
        
    } while(op != 9);
    
    return 0;
}

void mostrarMenu() {
    printf("\n=== MENU ===\n");
    printf("1. Agregar vehiculo\n");
    printf("2. Mostrar vehiculos\n");
    printf("3. Agregar cliente\n");
    printf("4. Mostrar clientes\n");
    printf("5. Buscar vehiculo\n");
    printf("6. Registrar venta\n");
    printf("7. Mostrar ventas\n");
    printf("8. Total ventas\n");
    printf("9. Salir\n");
}

void buscarVPers() {
    char mar[50];
    char tip[20];
    float Pmax;
    char r;
    
    printf("\n=== BUSQUEDA ===\n");
    printf("Filtrar por marca? (s/n): ");
    scanf("%c", &r);
    getchar();
    
    if(r == 's' || r == 'S') {
        printf("Marca: ");
        fgets(mar, 50, stdin);
        mar[strcspn(mar, "\n")] = 0;
    } else {
        strcpy(mar, "");
    }
    
    printf("Filtrar por tipo? (s/n): ");
    scanf("%c", &r);
    getchar();
    
    if(r == 's' || r == 'S') {
        printf("Tipo (nuevo/usado): ");
        fgets(tip, 20, stdin);
        tip[strcspn(tip, "\n")] = 0;
    } else {
        strcpy(tip, "");
    }
    
    printf("Filtrar por precio max? (s/n): ");
    scanf("%c", &r);
    getchar();
    
    if(r == 's' || r == 'S') {
        printf("Precio max: ");
        scanf("%f", &Pmax);
        getchar();
    } else {
        Pmax = 0;
    }
    
    buscarVCrit(mar, tip, Pmax);
}