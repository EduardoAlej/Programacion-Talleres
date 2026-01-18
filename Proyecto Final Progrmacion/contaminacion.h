#ifndef CONTAMINACION_H
#define CONTAMINACION_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define NUM_ZONAS 5
#define NUM_CONTAMINANTES 4
#define DIAS_SIMULACION 30
#define UMBRAL_DECRECIMIENTO 5
#define UMBRAL_CRECIMIENTO 7
#define ARCHIVO_PRINCIPAL "datos_contaminacion.dat"
#define ARCHIVO_HISTORICO "historico_semanal.dat"
#define DIAS_PARA_COPIA 7
#define MAX_CAMBIO 5.0
#define MIN_CAMBIO 1.0

// Nombres de los contaminantes
extern char *nombres_contaminantes[NUM_CONTAMINANTES];
extern float limites_oms[NUM_CONTAMINANTES];

// Estructura para un día específico
typedef struct {
    float valores[NUM_CONTAMINANTES];
    int tendencia[NUM_CONTAMINANTES]; // 1 = sube, -1 = baja, 0 = estable
    int dias_consecutivos[NUM_CONTAMINANTES]; // Días con misma tendencia
    char fecha[11]; // Formato YYYY-MM-DD
} RegistroDia;

// Estructura para una zona urbana
typedef struct {
    int id;
    char nombre[50];
    int poblacion;
    float latitud;
    float longitud;
    RegistroDia historico[DIAS_SIMULACION];
    float factor_contaminacion; // Factor específico por zona
} Zona;

// Estructura para factores climáticos
typedef struct {
    float temperatura;
    float velocidad_viento;
    float humedad;
    float presion_atmosferica;
} Clima;

// Estructura para predicción
typedef struct {
    Zona *zona;
    Clima clima_actual;
    float prediccion_24h[NUM_CONTAMINANTES];
    char alerta[200];
    int nivel_alerta; // 0 = normal, 1 = precaución, 2 = alerta, 3 = emergencia
} Prediccion;

// Estructura para probabilidades de cambio
typedef struct {
    float base;               // Probabilidad base
    float ajuste_tendencia;   // Ajuste por tendencia
    float ajuste_clima;       // Ajuste por clima
    float ajuste_zona;        // Ajuste por tipo de zona
} ProbabilidadCambio;

// Funciones de inicialización y memoria
void inicializar_sistema(Zona **zonas, int *num_zonas);
void liberar_memoria(Zona **zonas, int num_zonas);

// Funciones de simulación de datos
void simular_datos(Zona *zona, int dia_actual, Clima clima);
Clima generar_clima_aleatorio(void);
ProbabilidadCambio calcular_probabilidades(Zona *zona, int contaminante, int dia_anterior, Clima clima);
float calcular_magnitud_cambio(Zona *zona, int contaminante, int dia_anterior, Clima clima);

// Funciones de predicción y alertas
float calcular_prediccion(Zona *zona, int contaminante, Clima clima);
void generar_alerta(Prediccion *pred);

// Funciones de persistencia
void guardar_copia_historica(Zona *zona, int dia_actual);

// Funciones de entrada/salida seguras
int leer_entero(char *mensaje);
float leer_float(char *mensaje);
void limpiar_buffer(void);
void leer_cadena(char *mensaje, char *buffer, int tamanio);

// Funciones de visualización
void mostrar_promedios(Zona *zona);
void mostrar_detalle_zona(Zona *zona);
void mostrar_tendencias_detalladas(Zona *zona);

#endif // CONTAMINACION_H