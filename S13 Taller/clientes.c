#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clientes.h"

void agregarC() {
    FILE* f;
    Cliente c;
    
    f = fopen("clientes.dat", "ab");
    if(f == NULL) {
        printf("Error.\n");
        return;
    }
    
    c.idc = getLastIDC() + 1;
    
    printf("\n=== CLIENTE NUEVO ===\n");
    printf("ID:%d\n", c.idc);
    
    printf("Nombre: ");
    fgets(c.nombre, 100, stdin);
    c.nombre[strcspn(c.nombre, "\n")] = 0;
    
    printf("Edad: ");
    scanf("%d", &c.edad);
    getchar();
    
    printf("Presupuesto: ");
    scanf("%f", &c.presup);
    getchar();
    
    fwrite(&c, sizeof(Cliente), 1, f);
    fclose(f);
    
    printf("\nCliente agregado.\n");
}

void mostrarC() {
    FILE* f;
    Cliente c;
    
    f = fopen("clientes.dat", "rb");
    if(f == NULL) {
        printf("No hay.\n");
        return;
    }
    
    printf("\n=== CLIENTES ===\n");
    
    while(fread(&c, sizeof(Cliente), 1, f)) {
        printf("ID:%d Nombre:%s Edad:%d Presup:$%.2f\n",
               c.idc, c.nombre, c.edad, c.presup);
    }
    
    fclose(f);
}

Cliente* buscarCID(int id) {
    FILE* f;
    static Cliente c;
    int ok = 0;
    
    f = fopen("clientes.dat", "rb");
    if(f == NULL) {
        return NULL;
    }
    
    while(fread(&c, sizeof(Cliente), 1, f)) {
        if(c.idc == id) {
            ok = 1;
            break;
        }
    }
    
    fclose(f);
    
    if(ok) {
        return &c;
    } else {
        return NULL;
    }
}

int getLastIDC() {
    FILE* f;
    Cliente c;
    int last = 0;
    
    f = fopen("clientes.dat", "rb");
    if(f == NULL) {
        return 0;
    }
    
    fseek(f, -sizeof(Cliente), SEEK_END);
    if(fread(&c, sizeof(Cliente), 1, f)) {
        last = c.idc;
    }
    
    fclose(f);
    return last;
}