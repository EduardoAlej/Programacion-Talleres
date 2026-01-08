#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehiculos.h"

void agregarV() {
    FILE* f;
    Vehiculo v;
    
    f = fopen("vehiculos.dat", "ab");
    if(f == NULL) {
        printf("Error.\n");
        return;
    }
    
    v.idv = getLastID() + 1;
    
    printf("\n=== AGREGAR ===\n");
    printf("ID: %d\n", v.idv);
    
    printf("Marca: ");
    fgets(v.marca, 50, stdin);
    v.marca[strcspn(v.marca, "\n")] = 0;
    
    printf("Modelo: ");
    fgets(v.modelo, 50, stdin);
    v.modelo[strcspn(v.modelo, "\n")] = 0;
    
    printf("Ano: ");
    scanf("%d", &v.ano);
    getchar();
    
    printf("Precio: ");
    scanf("%f", &v.precio);
    getchar();
    
    printf("Tipo: ");
    fgets(v.tipo, 20, stdin);
    v.tipo[strcspn(v.tipo, "\n")] = 0;
    
    v.disp = 1;
    fwrite(&v, sizeof(Vehiculo), 1, f);
    fclose(f);
    
    printf("\nAgregado!\n");
}

void mostrarV() {
    FILE* f;
    Vehiculo v;
    
    f = fopen("vehiculos.dat", "rb");
    if(f == NULL) {
        printf("No hay.\n");
        return;
    }
    
    printf("\n=== VEHICULOS ===\n");
    
    while(fread(&v, sizeof(Vehiculo), 1, f)) {
        printf("ID:%d %s %s %d $%.2f %s %s\n",
               v.idv, v.marca, v.modelo, v.ano, 
               v.precio, v.tipo, v.disp ? "SI" : "NO");
    }
    
    fclose(f);
}

void buscarVID(int id) {
    FILE* f;
    Vehiculo v;
    int ok = 0;
    
    f = fopen("vehiculos.dat", "rb");
    if(f == NULL) {
        printf("Error.\n");
        return;
    }
    
    while(fread(&v, sizeof(Vehiculo), 1, f)) {
        if(v.idv == id) {
            printf("\n=== ENCONTRADO ===\n");
            printf("ID:%d\nMarca:%s\nModelo:%s\nAno:%d\nPrecio:$%.2f\nTipo:%s\nDisp:%s\n",
                   v.idv, v.marca, v.modelo, v.ano, 
                   v.precio, v.tipo, v.disp ? "SI" : "NO");
            ok = 1;
            break;
        }
    }
    
    if(!ok) {
        printf("\nID %d no existe.\n", id);
    }
    
    fclose(f);
}

void actDisp(int id, int est) {
    FILE* f;
    FILE* t;
    Vehiculo v;
    
    f = fopen("vehiculos.dat", "rb");
    t = fopen("temp.dat", "wb");
    
    if(f == NULL || t == NULL) {
        printf("Error.\n");
        return;
    }
    
    while(fread(&v, sizeof(Vehiculo), 1, f)) {
        if(v.idv == id) {
            v.disp = est;
        }
        fwrite(&v, sizeof(Vehiculo), 1, t);
    }
    
    fclose(f);
    fclose(t);
    
    remove("vehiculos.dat");
    rename("temp.dat", "vehiculos.dat");
}

void buscarVCrit(char* mar, char* tip, float Pmax) {
    FILE* f;
    Vehiculo v;
    int cnt = 0;
    
    f = fopen("vehiculos.dat", "rb");
    if(f == NULL) {
        printf("No hay.\n");
        return;
    }
    
    printf("\n=== RESULTADOS ===\n");
    
    while(fread(&v, sizeof(Vehiculo), 1, f)) {
        int ok = 1;
        
        if(strlen(mar) > 0 && strcasecmp(v.marca, mar) != 0) {
            ok = 0;
        }
        
        if(strlen(tip) > 0 && strcasecmp(v.tipo, tip) != 0) {
            ok = 0;
        }
        
        if(Pmax > 0 && v.precio > Pmax) {
            ok = 0;
        }
        
        if(v.disp == 0) {
            ok = 0;
        }
        
        if(ok) {
            printf("ID:%d %s %s $%.2f\n",
                   v.idv, v.marca, v.modelo, v.precio);
            cnt++;
        }
    }
    
    if(cnt == 0) {
        printf("\nNo hay.\n");
    } else {
        printf("\nEncontrados:%d\n", cnt);
    }
    
    fclose(f);
}

int getLastID() {
    FILE* f;
    Vehiculo v;
    int last = 0;
    
    f = fopen("vehiculos.dat", "rb");
    if(f == NULL) {
        return 0;
    }
    
    fseek(f, -sizeof(Vehiculo), SEEK_END);
    if(fread(&v, sizeof(Vehiculo), 1, f)) {
        last = v.idv;
    }
    
    fclose(f);
    return last;
}