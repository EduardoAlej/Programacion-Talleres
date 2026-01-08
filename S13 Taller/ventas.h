#ifndef VENTAS_H
#define VENTAS_H

typedef struct {
    int idventa;
    int idc;
    int idv;
    char fecha[11];
    float monto;
} Venta;

void regVenta();
void mostrarVent();
float calcTotal();

#endif