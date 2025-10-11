#include <stdio.h>
#include <stdlib.h>
#define Estudiantes 5
#define Materias 3

int main(){
    float NN[Materias][Estudiantes];
    float Nota;
    int Buffer;
    int Vc;
    printf("Este es un programa de notas de 3 materias y 5 estudiantes\n");
    printf("Inserte cualquier tecla para continuar: ");
    getchar();
    printf("\n\n");  
    for(int i = 0; i < Materias; i++) {
        printf("=== Materia %d ===\n", i + 1);
        for(int j = 0; j < Estudiantes; j++) {
            Vc=0;
            do {
                printf("Inserte la nota del Estudiante %d (0-10): ", j + 1);
                scanf("%f", &Nota);
                if (Nota < 0 || Nota > 10 ) {
                    printf("Error: Ingrese un numero valido\n");
                }else{
                    Vc=1;
                }
                
            } while (!Vc);
            
            NN[i][j] = Nota;

        }
        printf("\n");
    }
    
    printf("\n\n\n=== Promedios por estudiante ===\n\n");
    
    for(int j = 0; j < Estudiantes; j++) {
        float suma = 0;
        for(int i = 0; i < Materias; i++) {
            suma += NN[i][j];
        }
        float promedio = suma / Materias;
        printf("Estudiante %d: Promedio %.2f - ", j + 1, promedio);
        if (promedio >= 6) {
            printf("Paso\n");
        } else {
            printf("No paso\n");
        }
    }
    
    printf("\nPresione Enter para salir...");
    getchar();
    return 0;
}