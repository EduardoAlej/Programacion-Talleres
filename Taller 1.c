#include <stdio.h>
#include <stdlib.h>
int main() {
    int d, n, i, vv, ID= 1131;
    float Papas =0, c=0, v=1.5; 
    fflush(stdin);
    printf("Cuantos dias quieres logear\n");
    scanf("%d", &d);
    int dia =1;
    do {
        printf("Es el dia %d\n", dia);
        dia ++;
        d = d - 1;
         int inputcorrecto =0;
        while (!inputcorrecto){
            Inicio:
        printf("El ID de las Papas es 1131\n");
        printf("Escriba lo que quiere hacer, vender (1), insertar producto (2), inventario (3), cuentas (4), si quiere terminar el dia (5), si quiere cancelar el logeo presione (6)\n");
        scanf("%d", &n);
        if (n < 5){
        if (Papas == 0 && n == 1) {
            printf("No hay producto se va a insertar producto automaticamente\n");
            n = 2; 
        }
        }
        switch(n) {
            case 1:
                printf("Inserte el ID del producto\n");
                scanf("%d", &ID);
                if (ID==1131){
                    printf("El producto papa tiene un precio de 1 dolar con 50 centavos\n");
                } else{
                    printf ("ID incorrecto regresando al menu\n");
                    goto Inicio;
                }
                printf("Hay %f en el inventario\n", Papas);
                printf("Cuanto quiere vender\n");
                scanf("%d", &i);
                if (i<0){
                    printf("Valor imposible regrese a menu\n");
                } else {
                if (Papas > 0 && Papas >= i) {
                    Papas = Papas - i;
                    printf("Vendio %d de producto\n", i);
                    do {
                        c = c + v;
                        i = i - 1;
                    } while (i > 0);
                printf("por %f dolares\n", c);
                printf("Quiere seguir logeando escriba 1 si aun quiere y escriba otro numero si quiere terminar\n");
                scanf("%d", &vv);
                if (vv==1){
                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                } else {
                    printf("No hay suficiente inventario\n");
                }
            }
                break;
                
            case 2:
                printf("Inserte el ID del producto\n");
                scanf("%d", &ID);
                if (ID==1131){
                    printf("Esta insertando el producto Papa\n");
                } else{
                    printf ("ID incorrecto regresando al menu\n");
                    goto Inicio;
                }
                printf("Cuanto quiere insertar\n");
                scanf("%d", &i);
                if ( i<0 || i ==0){
                    printf("Valor imposible se regresa a menu\n");
                } else {
                    Papas = Papas + i;
                printf("Metio %d de producto\n", i);
                printf("Quiere seguir logeando escriba 1 si aun quiere y escriba otro numero si quiere terminar\n");
                scanf("%d", &vv);
                if (vv==1){
                    
                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                }
                break;
                
            case 3:
            
                printf("Hay %f en el inventario\n", Papas);
                printf("Quiere seguir logeando escriba 1 si aun quiere y escriba otro numero si quiere terminar\n");
                scanf("%d", &vv);
                if (vv==1){
                  
                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                break;
                
            case 4:
                printf("Ha vendido %f dolares\n", c);
                printf("Quiere seguir logeando escriba 1 si aun quiere y escriba otro numero si quiere terminar\n");
                scanf("%d", &vv);
                if (vv==1){

                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                break;
            case 5:
                 printf("Seguro que quiere terminar el dia? Presione un numero diferente a 1 si quiere confirmar, para cancelar oprima 1\n");
                scanf("%d", &vv);
                  if (vv==1){
                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                break;
                case 6:
                printf("Seguro que quiere cancelar el logeo? Presione un numero diferente a 1 si quiere confirmar, para cancelar oprima 1\n");
                scanf("%d", &vv);
                  if (vv==1){
                inputcorrecto =1;
                d=-1;
                }else{
                inputcorrecto=0;
                }
            default:
                printf("Opcion inexistente intente de nuevo\n");
        }
        }
    } while (d > 0);
     system("cls");
     printf("Resumen\n");
    printf("Hay %f productos en el inventario\n", Papas);
    printf("Ha vendido %f dolares\n", c);
    return 0;
}