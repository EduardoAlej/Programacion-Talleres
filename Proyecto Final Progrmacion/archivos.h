#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "contaminacion.h"

// ==================== FUNCIONES DE MANEJO DE ARCHIVOS ====================

// Guardar datos en archivo binario
void guardar_datos_binario(Zona *zonas, int num_zonas);

// Cargar datos desde archivo binario
void cargar_datos_binario(Zona **zonas, int *num_zonas);

// Guardar reporte en formato CSV
void guardar_reporte_csv(Zona *zonas, int num_zonas, const char *nombre_archivo);

// Generar reporte de alertas en formato texto
void generar_reporte_alerta(Zona *zonas, int num_zonas, Prediccion *predicciones);

// Guardar configuración del sistema
void guardar_configuracion(Zona *zonas, int num_zonas);

// Cargar configuración del sistema
void cargar_configuracion(Zona **zonas, int *num_zonas);

// Leer archivo histórico
void leer_archivo_historico(void);

// Limpiar archivo histórico
void limpiar_archivo_historico(void);

// Exportar datos a formato JSON
void exportar_a_json(Zona *zonas, int num_zonas, const char *nombre_archivo);

#endif // ARCHIVOS_H