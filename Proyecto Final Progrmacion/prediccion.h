#ifndef PREDICCION_H
#define PREDICCION_H

#include "contaminacion.h"

// ==================== FUNCIONES DE PREDICCIÓN ====================

// Calcular predicción para un contaminante específico
float calcular_prediccion(Zona *zona, int contaminante, Clima clima);

// Generar alerta basada en predicciones
void generar_alerta(Prediccion *pred);

// Calcular predicciones para todos los contaminantes de una zona
void calcular_predicciones_zona(Zona *zona, Clima clima, float predicciones[NUM_CONTAMINANTES]);

// Calcular factor de riesgo de una zona
float calcular_factor_riesgo(Zona *zona);

// Obtener recomendaciones específicas
void obtener_recomendaciones(Zona *zona, float predicciones[NUM_CONTAMINANTES], char recomendaciones[5][200], int *num_recomendaciones);

// Calcular índice de calidad del aire (AQI)
float calcular_indice_calidad_aire(Zona *zona);

// Verificar si se requiere acción inmediata
int requiere_accion_inmediata(Zona *zona, float predicciones[NUM_CONTAMINANTES]);

// Calcular tendencia futura
int calcular_tendencia_futura(Zona *zona, int contaminante, Clima clima);

// Obtener nivel de riesgo (texto)
const char* obtener_nivel_riesgo(int nivel);

// Obtener color para nivel de alerta
const char* obtener_color_alerta(int nivel);

// Obtener icono para nivel de alerta
const char* obtener_icono_alerta(int nivel);

#endif // PREDICCION_H