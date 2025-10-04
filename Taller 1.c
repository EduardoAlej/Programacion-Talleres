#include <stdio.h>
#include <stdlib.h>
int main() {
    int v = 1;
    int d, n, i, p = 0, c = 0, vv;
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
        printf("Escriba lo que quiere hacer, vender (1), insertar producto (2), inventario (3), cuentas (4), si quiere terminar el dia (5), si quiere cancelar el logeo presione (6)\n");
        scanf("%d", &n);
        if (n < 5){
        if (p == 0 && n == 1) {
            printf("No hay producto se va a insertar producto automaticamente\n");
            n = 2; 
        }
        }
        switch(n) {
            case 1:
                printf("Cuanto quiere vender\n");
                scanf("%d", &i);
                if (i<0){
                    printf("Valor imposible regrese a menu");
                } else {
                if (p > 0 && p >= i) {
                    p = p - i;
                    printf("vendio %d de producto\n", i);
                    
                    do {
                        c = c + v;
                        i = i - 1;
                    } while (i > 0);
                printf("Quiere seguir logeando escriba 1 si aun quiere y escriba otro numero si quiere terminar");
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
          
                printf("Cuanto quiere insertar\n");
                scanf("%d", &i);
                if ( i<0 || i ==0){
                    printf("Valor imposible se regresa a menu");
                } else {
                    p = p + i;
                printf("Metio %d de producto\n", i);
                printf("Quiere seguir logeando escriba 1 si aun quiere y escriba otro numero si quiere terminar");
                scanf("%d", &vv);
                if (vv==1){
                    
                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                }
                break;
                
            case 3:
            
                printf("Hay %d en el inventario\n", p);
                printf("Quiere seguir logeando escriba 1 si aun quiere y escriba otro numero si quiere terminar");
                scanf("%d", &vv);
                if (vv==1){
                  
                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                break;
                
            case 4:
                printf("Ha vendido %d dolares\n", c);
                printf("Quiere seguir logeando escriba 1 si aun quiere y escriba otro numero si quiere terminar");
                scanf("%d", &vv);
                if (vv==1){

                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                break;
            case 5:
                 printf("Seguro que quiere terminar el dia? Presione un numero diferente a 1 si quiere confirmar, para cancelar oprima 1");
                scanf("%d", &vv);
                  if (vv==1){
                inputcorrecto =0;
                }else{
                inputcorrecto=1;
                }
                break;
                case 6:
                printf("Seguro que quiere cancelar el logeo? Presione un numero diferente a 1 si quiere confirmar, para cancelar oprima 1");
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
    printf("Hay %d productos en el inventario\n", p);
    printf("Ha vendido %d dolares\n", c);
    return 0;
}