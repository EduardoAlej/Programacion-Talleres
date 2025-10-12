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
        for(int j = 0; j < Estudiantes; j++) {
            NN[i][j] = 0.0;
        }
    }
     
    for(int i = 0; i < Materias; i++) {
        printf("Materia %d \n", i + 1);
        for(int j = 0; j < Estudiantes; j++) {
            Vc=0;
            do {
                printf("Inserte la nota del Estudiante %d (del 0 al 10): ", j + 1);
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
    
    printf("\n\nPromedios por estudiante\n");
    
    for(int j = 0; j < Estudiantes; j++) {
        float suma = 0;
        for(int i = 0; i < Materias; i++) {
            suma += NN[i][j];
        }
        float promedio = suma / Materias;
        printf("Estudiante %d: Promedio %.2f  ", j + 1, promedio);
        printf("\n");
    }
    printf("\n\nPromedios por materia\n");
    for(int i = 0; i < Materias; i++) {
        float suma = 0;
        for(int j = 0; j < Estudiantes; j++) {
            suma += NN[i][j];
        }
        float promediom =suma/Estudiantes;
        printf("El promedio del grado es %.2f en la materia %d\n", promediom, i + 1);
    }

    printf("\n\nNota mas alta y baja por estudiante\n");
    for(int j = 0; j < Estudiantes; j++) {
        float max_est = NN[0][j];
        float min_est = NN[0][j];
        
        for(int i = 1; i < Materias; i++) {
            if (NN[i][j] > max_est) {
                max_est = NN[i][j];
            }
            if (NN[i][j] < min_est) {
                min_est = NN[i][j];
            }
        }
        printf("Estudiante %d: Max=%.2f, Min=%.2f\n", j + 1, max_est, min_est);
    }

    printf("\n\nNota mas alta y baja por materia\n");
    for(int i = 0; i < Materias; i++) {
        float max_mat = NN[i][0];
        float min_mat = NN[i][0];
        
        for(int j = 1; j < Estudiantes; j++) {
            if (NN[i][j] > max_mat) {
                max_mat = NN[i][j];
            }
            if (NN[i][j] < min_mat) {
                min_mat = NN[i][j];
            }
        }
        printf("Materia %d: Max=%.2f, Min=%.2f\n", i + 1, max_mat, min_mat);
    }

    printf("\n\nEstudiantes aprobados y reprobados\n");
    for(int i = 0; i < Materias; i++) {
        int aprobados = 0;
        int reprobados = 0;
        
        for(int j = 0; j < Estudiantes; j++) {
            if (NN[i][j] >= 6) {
                aprobados++;
            } else {
                reprobados++;
            }
        }
        printf("Materia %d: %d aprobados, %d reprobados\n", i + 1, aprobados, reprobados);
    }

    printf("\nPresione para salir");
    getchar();
    return 0;
}