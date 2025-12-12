#include <stdio.h>
#define MAX_ESTUDIANTES 50

int main() {
    int numEstudiantes;
    float calificaciones[3];
    float promedios[MAX_ESTUDIANTES];
    int I;
    float promasa=0;
    float promasb=0;
    In:
        printf("Este es un programa para notas de estudiantes\n");
        printf("Cuantos estudiantes quiere calificar, maximo 50\n");
        scanf("%i",&I);
        if(I<1||I>50){
            printf("Opcion invalida intente de nuevo\n");
            goto In;
        }
        while(getchar() != '\n' && getchar() != EOF); 
        int x=0;
        int j=0;
 while(x<I){
    promedios[x] = 0.0f;
    for(j = 0; j < 3; j++){
    rep:
    printf("inserte la calificacion %i del estudiante %i, no se olvide maximo 100\n",j+1,x+1);
    if (scanf("%f",&calificaciones[j]) != 1) {
        int _ch;
        while((_ch = getchar()) != '\n' && _ch != EOF);
        printf("Entrada invalida, intente de nuevo\n");
        goto rep;
    }
    {
        int _ch;
        while((_ch = getchar()) != '\n' && _ch != EOF);
    }
    if(calificaciones[j]<0 || calificaciones[j]>100){
        printf("Opcion invalida intente de nuevo\n");
        goto rep;
    }else{

        promedios[x] = promedios[x] + calificaciones[j];
    }
}
for(int z; promedios[z];z++){
    if( promasa<promedios[z]){
    promasa = promedios[z];
    }
}
 for(int s; promedios[s];s++){
    if( promasb>promedios[s]){
    promasb = promedios[s];
    }
    printf("promedio mas bajo %f",promasa);
}   
    promedios[x] = promedios[x] / 3.0f;
x++;
}
printf("promedio mas alto %f",promasa);
printf("promedio mas alto %f",promasb);
printf("promedio general %f",promedios);


    // TU CÓDIGO AQUÍ
    // Paso 1: Pedir número de estudiantes (validar 1-50)
    
    // Paso 2: Para cada estudiante, pedir 3 calificaciones
    //         Validar que cada calificación sea 0-100
    //         Calcular promedio y guardar en array promedios[]
    
    // Paso 3: Calcular:
    //         - Promedio más alto (usar for)
    //         - Promedio más bajo (usar while)
    //         - Promedio general
    
    // Paso 4: Mostrar resultados
    return 0;
}