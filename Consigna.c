#include <stdio.h>
#include <string.h>
#define Mproducto 5
#define Mlong 50

void limpiarBuffer(){
    int c;
    while ((c=getchar())!= '\n' && c !=EOF);
}

int BP(char nombres[Mproducto][Mlong], int contador, char NB[]){
    for(int i=0; i< contador;i++){
        int j=0;
        while(nombres[i][j] != '\0' && NB[j] != '\0' && nombres[i][j]==NB[j]){
            j++;
        }
        if (nombres[i][j]== '\0' && NB[j]=='\0'){
            return i;
        }
    }
    return -1;
}

int main(){
    char nombres[Mproducto][Mlong];
    int cantidad[Mproducto];
    float tiempo[Mproducto];
    float recursos[Mproducto];
    int Cproducto=0, opcion, i;
    
    do{
        printf("Sistema de productos de fabrica\n Escriba el numero segun lo que requiere\n 1: Ingresar producto\n 2: Editar el producto\n 3: Eliminar un producto\n 4:Revisar con limites si se puede fabricar un producto\n 5: Salir del programa\n ");
        if (scanf("%d", &opcion) !=1){
            printf("Entrado no valida\n");
            limpiarBuffer();
            continue;
        }
        limpiarBuffer();
        
        switch(opcion){
            case 1:
                if(Cproducto>= Mproducto){
                    printf("Se paso del maximo de 5 productos\n");
                    break;
                }
                printf("Ingrese el nombre del producto\n");
                fgets(nombres[Cproducto], Mlong, stdin);
                nombres[Cproducto][strcspn(nombres[Cproducto],"\n")]='\0';
                if(nombres[Cproducto][0]=='\0'){
                    printf("El nombre no puede estar vacio\n");
                    break;
                }
                printf("Cuantos productos hay que hacer:\n");
                if (scanf("%d", &cantidad[Cproducto])!=1 || cantidad[Cproducto] <=0){  // Cambiado Mproducto por Cproducto
                    printf("Debe de ser un numero positivo\n");
                    limpiarBuffer();
                    break;
                }
                printf("Cuanto tiempo se demora (horas):\n");
                if (scanf("%f", &tiempo[Cproducto])!=1 || tiempo[Cproducto] <=0){  // Cambiado %d por %f
                    printf("Debe de ser un numero positivo\n");
                    limpiarBuffer();
                    break;
                }
                printf("Cuantos recursos se usan para la produccion (toneladas):\n");
                if (scanf("%f", &recursos[Cproducto])!=1 || recursos[Cproducto] <=0){  // Cambiado %d por %f
                    printf("Deve de ser un numero positivo\n");
                    limpiarBuffer();
                    break;
                }
                limpiarBuffer();
                Cproducto++;
                break;
                
            case 2:
                if(Cproducto==0){
                    printf("No hay productos");
                    break;
                }
                printf("Productos ingresados:\n");
                for(i=0; i<Cproducto; i++){
                    printf("%d. %s\n",i+1, nombres[i]);
                }
                char nombreEdi[Mlong];
                printf("\n Ingrese el nombre del producto que quiere editar:\n");
                fgets(nombreEdi, Mlong, stdin);
                nombreEdi[strcspn(nombreEdi, "\n")]= '\0';
                
                if (nombreEdi[0] == '\0') {
                    printf("El nombre no puede estar vacio.\n");
                    break;
                }
                int BusquedaEdit= BP( nombres, Cproducto, nombreEdi);  // Cambiado contador por Cproducto
                if(BusquedaEdit==-1){
                    printf("Producto no encontrado.\n");
                    break;
                }
                printf("Cual es el nuevo nombre.\n");
                fgets(nombres[BusquedaEdit],Mlong, stdin );
                nombres[BusquedaEdit][strcspn(nombres[BusquedaEdit],"\n")]= '\0';
                if (nombres[BusquedaEdit][0] == '\0') {
                    printf(" El nombre no puede estar vacio.\n");
                    break;
                } 
                printf(" Inserte la nueva cantidad: ");
                if (scanf("%d", &cantidad[BusquedaEdit]) != 1 || cantidad[BusquedaEdit] <= 0) {
                    printf(" La cantidad debe ser un numero positivo.\n");
                    limpiarBuffer();
                    break;
                }
                printf(" Inserte el nuevo tiempo: ");
                if (scanf("%f", &tiempo[BusquedaEdit]) != 1 || tiempo[BusquedaEdit] <= 0) {  // %d por %f
                    printf(" El tiempo debe ser un numero positivo.\n");
                    limpiarBuffer();
                    break;
                }
                printf(" Inserte los nuevos recursos: ");
                if (scanf("%f", &recursos[BusquedaEdit]) != 1 || recursos[BusquedaEdit] <= 0) {  // %d por %f
                    printf(" Los recursos deben ser un numero positivo.\n");
                    limpiarBuffer();
                    break;
                }
                limpiarBuffer();
                break;
                
            case 3:
                if(Cproducto==0){
                    printf("No hay productos");
                    break;
                }
                printf("Productos ingresados:\n");
                for(i=0; i<Cproducto; i++){
                    printf("%d. %s\n",i+1, nombres[i]);
                }
                char nombreEli[Mlong];
                printf("\n Ingrese el nombre del producto que quiere eliminar:\n");
                fgets(nombreEli, Mlong, stdin);
                nombreEli[strcspn(nombreEli, "\n")]= '\0';
                if (nombreEli[0] == '\0') {
                    printf(" El nombre no puede estar vacio.\n");
                    break;
                }
                int BusquedaEli= BP( nombres, Cproducto, nombreEli);  // Cambiado contador por Cproducto
                if(BusquedaEli==-1){
                    printf("Producto no encontrado.\n");
                    break;
                }
                if(BusquedaEli != Cproducto-1){
                    strcpy(nombres[BusquedaEli],nombres[Cproducto-1]);
                    cantidad[BusquedaEli]= cantidad[Cproducto-1];
                    tiempo[BusquedaEli]=tiempo[Cproducto-1];
                    recursos[BusquedaEli]=recursos[Cproducto-1];  // Agregado ;
                }
                Cproducto--;
                break;
                
            case 4:
                if(Cproducto==0){
                    printf("No hay productos");
                    break;
                }
                float tiempoTotal = 0;
                for (i = 0; i < Cproducto; i++) {
                    tiempoTotal += cantidad[i] * tiempo[i];
                }
                printf("Tiempo total de fabricacion requerido: %.2f horas\n", tiempoTotal);
                
                float recursosTotal = 0;
                for (i = 0; i < Cproducto; i++) {
                    recursosTotal += cantidad[i] * recursos[i];
                }
                printf("Cantidad de recursos necesarios: %.2f toneladas\n", recursosTotal);
                
                float limiteTiempo, limiteRecursos;
                printf("\nIngrese el limite de tiempo: ");
                if (scanf("%f", &limiteTiempo) != 1 || limiteTiempo <= 0) {
                    printf("El limite de tiempo debe ser un numero positivo.\n");
                    limpiarBuffer();
                    break;
                }
                
                printf("Ingrese el limite de recursos: ");
                if (scanf("%f", &limiteRecursos) != 1 || limiteRecursos <= 0) {
                    printf("El limite de recursos debe ser un numero positivo.\n");
                    limpiarBuffer();
                    break;
                }
                
                limpiarBuffer();
                if (tiempoTotal <= limiteTiempo && recursosTotal <= limiteRecursos) {
                    printf("La fabrica puede cumplir con la demanda\n");
                } else {
                    printf("La fabrica no puede cumplir con la demanda\n");
                }
                printf("\n");
                break;
                
            case 5:
                printf("Saliendo del programa\n");
                break;
                
            default:
                printf("Opcion no valida.\n");
        }
    }while(opcion !=5);
    return 0;
}