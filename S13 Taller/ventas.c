#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ventas.h"
#include "vehiculos.h"
#include "clientes.h"

void regVenta() {
    FILE* f;
    Venta v;
    int idc, idv;
    Cliente* c;
    
    printf("\n=== NUEVA VENTA ===\n");
    
    f = fopen("ventas.dat", "rb");
    if(f == NULL) {
        v.idventa = 1;
    } else {
        fseek(f, -sizeof(Venta), SEEK_END);
        if(fread(&v, sizeof(Venta), 1, f)) {
            v.idventa++;
        } else {
            v.idventa = 1;
        }
        fclose(f);
    }
    
    mostrarC();
    
    printf("\nID cliente: ");
    scanf("%d", &idc);
    getchar();
    
    c = buscarCID(idc);
    if(c == NULL) {
        printf("Cliente no.\n");
        return;
    }
    
    FILE* fv = fopen("vehiculos.dat", "rb");
    Vehiculo ve;
    
    if(fv != NULL) {
        while(fread(&ve, sizeof(Vehiculo), 1, fv)) {
            if(ve.disp) {
                printf("ID:%d - %s %s - $%.2f\n",
                       ve.idv, ve.marca, ve.modelo, ve.precio);
            }
        }
        fclose(fv);
    }
    
    printf("\nID vehiculo: ");
    scanf("%d", &idv);
    getchar();
    
    FILE* fv2 = fopen("vehiculos.dat", "rb");
    int ok = 0;
    float precioV = 0;
    
    if(fv2 != NULL) {
        while(fread(&ve, sizeof(Vehiculo), 1, fv2)) {
            if(ve.idv == idv && ve.disp) {
                ok = 1;
                precioV = ve.precio;
                break;
            }
        }
        fclose(fv2);
    }
    
    if(!ok) {
        printf("Vehiculo no.\n");
        return;
    }
    
    if(c->presup < precioV) {
        printf("No hay presupuesto.\n");
        printf("Presup:$%.2f Precio:$%.2f\n", c->presup, precioV);
        return;
    }
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(v.fecha, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    
    v.idc = idc;
    v.idv = idv;
    v.monto = precioV;
    
    f = fopen("ventas.dat", "ab");
    if(f == NULL) {
        printf("Error.\n");
        return;
    }
    
    fwrite(&v, sizeof(Venta), 1, f);
    fclose(f);
    
    actDisp(idv, 0);
    
    printf("\nVenta OK!\n");
    printf("ID Venta:%d\nCliente:%s\nMonto:$%.2f\nFecha:%s\n",
           v.idventa, c->nombre, v.monto, v.fecha);
}

void mostrarVent() {
    FILE* f;
    Venta v;
    
    f = fopen("ventas.dat", "rb");
    if(f == NULL) {
        printf("No hay.\n");
        return;
    }
    
    printf("\n=== VENTAS ===\n");
    
    while(fread(&v, sizeof(Venta), 1, f)) {
        printf("ID:%d Cli:%d Veh:%d Fecha:%s Monto:$%.2f\n",
               v.idventa, v.idc, v.idv, v.fecha, v.monto);
    }
    
    fclose(f);
}

float calcTotal() {
    FILE* f;
    Venta v;
    float total = 0;
    
    f = fopen("ventas.dat", "rb");
    if(f == NULL) {
        return 0;
    }
    
    while(fread(&v, sizeof(Venta), 1, f)) {
        total += v.monto;
    }
    
    fclose(f);
    return total;
}