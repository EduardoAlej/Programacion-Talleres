#include <stdint.h>
#include "archivos.h"

typedef struct {
    uint32_t magic;    // 'CNTM'
    uint32_t version;  // 1
    uint32_t num_zonas;
} Header;

// ==================== GUARDAR DATOS BINARIOS ====================

void guardar_datos_binario(Zona *zonas, int num_zonas) {
    if (zonas == NULL || num_zonas <= 0) {
        printf("Error: Datos inválidos para guardar.\n");
        return;
    }

    FILE *archivo = fopen(ARCHIVO_PRINCIPAL, "wb");
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo '%s'.\n", ARCHIVO_PRINCIPAL);
        return;
    }

    Header hdr = { 0x434e544d, 1, (uint32_t)num_zonas };
    fwrite(&hdr, sizeof(hdr), 1, archivo);
    fwrite(zonas, sizeof(Zona), (size_t)num_zonas, archivo);
    fclose(archivo);
    printf("✓ Datos guardados en '%s' (%d zonas, %zu bytes).\n",
           ARCHIVO_PRINCIPAL, num_zonas, sizeof(Zona) * (size_t)num_zonas);
}

// ==================== CARGAR DATOS BINARIOS ====================

static int registro_valido(const Zona *z) {
    if (z->id <= 0 || z->poblacion < 0) return 0;
    if (!isfinite(z->latitud) || z->latitud < -90.0f || z->latitud > 90.0f) return 0;
    if (!isfinite(z->longitud) || z->longitud < -180.0f || z->longitud > 180.0f) return 0;
    if (!isfinite(z->factor_contaminacion) || z->factor_contaminacion < 0.1f || z->factor_contaminacion > 10.0f) return 0;
    for (int d=0; d<DIAS_SIMULACION; d++) {
        // Fecha ISO: YYYY-MM-DD
        if (strlen(z->historico[d].fecha) != 10 || z->historico[d].fecha[4] != '-' || z->historico[d].fecha[7] != '-') return 0;
        for (int c=0; c<NUM_CONTAMINANTES; c++) {
            if (!isfinite(z->historico[d].valores[c])) return 0;
            if (z->historico[d].valores[c] < 0.0f || z->historico[d].valores[c] > 5000.0f) return 0;
            if (z->historico[d].dias_consecutivos[c] < 0 || z->historico[d].dias_consecutivos[c] > 10000) return 0;
            if (z->historico[d].tendencia[c] < -1 || z->historico[d].tendencia[c] > 1) return 0;
        }
    }
    return 1;
}

static void regenerar_historico_zona(Zona *z) {
    // Regenera 30 días coherentes desde hoy hacia atrás usando la misma lógica de simulación
    // Dia 0: valor base, fecha (hoy - 29)
    time_t t = time(NULL);
    struct tm base = *localtime(&t);
    struct tm inicio = base;
    inicio.tm_mday -= (DIAS_SIMULACION - 1);
    mktime(&inicio);
    for (int c=0; c<NUM_CONTAMINANTES; c++) {
        z->historico[0].valores[c] = 30.0f + (float)(rand() % 40); // 30..69
        z->historico[0].tendencia[c] = 0;
        z->historico[0].dias_consecutivos[c] = 0;
    }
    strftime(z->historico[0].fecha, sizeof(z->historico[0].fecha), "%Y-%m-%d", &inicio);
    // Simula días 1..29 con clima aleatorio
    for (int d=1; d<DIAS_SIMULACION; d++) {
        Clima clima = generar_clima_aleatorio();
        simular_datos(z, d, clima);
    }
}

void cargar_datos_binario(Zona **zonas, int *num_zonas) {
    FILE *archivo = fopen(ARCHIVO_PRINCIPAL, "rb");
    if (archivo == NULL) {
        printf("Archivo '%s' no encontrado. Se creará nuevo sistema.\n", ARCHIVO_PRINCIPAL);
        return;
    }
    Header hdr;
    if (fread(&hdr, sizeof(hdr), 1, archivo) != 1 || hdr.magic != 0x434e544d) {
        printf("Error: Cabecera inválida.\n");
        fclose(archivo);
        return;
    }
    *num_zonas = (int)hdr.num_zonas;
    if (*num_zonas <= 0 || *num_zonas > 100) {
        printf("Error: Número de zonas inválido en archivo (%d).\n", *num_zonas);
        fclose(archivo);
        return;
    }
    *zonas = (Zona *)calloc((size_t)*num_zonas, sizeof(Zona));
    if (!*zonas) { printf("Error: Memoria insuficiente.\n"); fclose(archivo); return; }

    size_t leidos = fread(*zonas, sizeof(Zona), (size_t)*num_zonas, archivo);
    fclose(archivo);
    if (leidos != (size_t)*num_zonas) {
        printf("Advertencia: No se pudieron leer todos los datos (%zu/%d).\n", leidos, *num_zonas);
        free(*zonas); *zonas=NULL; *num_zonas=0; return;
    }

    // Sanitiza campos básicos
    for (int i = 0; i < *num_zonas; i++) {
        Zona *z = &(*zonas)[i];
        z->nombre[sizeof(z->nombre)-1] = '\0';
        if (!isfinite(z->latitud) || z->latitud < -90.0f || z->latitud > 90.0f) z->latitud = 0.0f;
        if (!isfinite(z->longitud) || z->longitud < -180.0f || z->longitud > 180.0f) z->longitud = 0.0f;
        if (!isfinite(z->factor_contaminacion) || z->factor_contaminacion < 0.5f || z->factor_contaminacion > 5.0f) z->factor_contaminacion = 1.0f;
        if (z->poblacion < 0) z->poblacion = 0;
    }

    // Valida y regenera históricos inválidos
    for (int i = 0; i < *num_zonas; i++) {
        Zona *z = &(*zonas)[i];
        if (!registro_valido(z)) {
            printf("⚠️  Zona %d con histórico inválido. Regenerando datos coherentes...\n", i+1);
            // Mantén metadatos básicos (id, nombre, poblacion, ubicacion, factor)
            regenerar_historico_zona(z);
        }
    }
    printf("✓ Datos cargados desde '%s' (%d zonas).\n", ARCHIVO_PRINCIPAL, *num_zonas);
}

// ==================== GUARDAR REPORTE CSV ====================

void guardar_reporte_csv(Zona *zonas, int num_zonas, const char *nombre_archivo) {
    if (zonas == NULL || num_zonas <= 0 || nombre_archivo == NULL) {
        printf("Error: Parámetros inválidos para generar CSV.\n");
        return;
    }
    
    // Verificar extensión .csv
    char nombre_completo[150];
    if (strstr(nombre_archivo, ".csv") == NULL) {
        snprintf(nombre_completo, sizeof(nombre_completo), "%s.csv", nombre_archivo);
    } else {
        strncpy(nombre_completo, nombre_archivo, sizeof(nombre_completo) - 1);
        nombre_completo[sizeof(nombre_completo) - 1] = '\0';
    }
    
    FILE *archivo = fopen(nombre_completo, "w");
    
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo '%s'.\n", nombre_completo);
        return;
    }
    
    // Cabecera con metadatos
    time_t ahora = time(NULL);
    struct tm *tm_info = localtime(&ahora);
    char fecha_reporte[20];
    strftime(fecha_reporte, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(archivo, "# Reporte de Contaminación Atmosférica\n");
    fprintf(archivo, "# Generado: %s\n", fecha_reporte);
    fprintf(archivo, "# Zonas: %d, Días: %d\n", num_zonas, DIAS_SIMULACION);
    fprintf(archivo, "# Contaminantes: CO2,SO2,NO2,PM2.5\n");
    fprintf(archivo, "# Límites OMS: %.2f,%.2f,%.2f,%.2f\n", 
            limites_oms[0], limites_oms[1], limites_oms[2], limites_oms[3]);
    fprintf(archivo, "#\n");
    
    // Encabezado de columnas
    fprintf(archivo, "Zona_ID,Zona_Nombre,Fecha");
    
    // Columnas para valores
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        fprintf(archivo, ",%s_Valor", nombres_contaminantes[c]);
    }
    
    // Columnas para tendencias
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        fprintf(archivo, ",%s_Tendencia", nombres_contaminantes[c]);
    }
    
    // Columnas para días consecutivos
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        fprintf(archivo, ",%s_Dias_Consecutivos", nombres_contaminantes[c]);
    }
    
    fprintf(archivo, ",Factor_Zona\n");
    
    // Escribir datos
    for (int z = 0; z < num_zonas; z++) {
        for (int d = 0; d < DIAS_SIMULACION; d++) {
            fprintf(archivo, "%d,\"%s\",%s",
                   zonas[z].id,
                   zonas[z].nombre,
                   zonas[z].historico[d].fecha);
            
            // Valores de contaminantes
            for (int c = 0; c < NUM_CONTAMINANTES; c++) {
                fprintf(archivo, ",%.2f", zonas[z].historico[d].valores[c]);
            }
            
            // Tendencias
            for (int c = 0; c < NUM_CONTAMINANTES; c++) {
                fprintf(archivo, ",%d", zonas[z].historico[d].tendencia[c]);
            }
            
            // Días consecutivos
            for (int c = 0; c < NUM_CONTAMINANTES; c++) {
                fprintf(archivo, ",%d", zonas[z].historico[d].dias_consecutivos[c]);
            }
            
            // Factor de zona
            fprintf(archivo, ",%.2f\n", zonas[z].factor_contaminacion);
        }
    }
    
    fclose(archivo);
    printf("✓ Reporte CSV guardado en '%s' (%d registros).\n", 
           nombre_completo, num_zonas * DIAS_SIMULACION);
}

// ==================== GENERAR REPORTE DE ALERTAS ====================

void generar_reporte_alerta(Zona *zonas, int num_zonas, Prediccion *predicciones) {
    if (zonas == NULL || num_zonas <= 0 || predicciones == NULL) {
        printf("Error: Datos inválidos para generar reporte de alertas.\n");
        return;
    }
    
    // Generar nombre de archivo con timestamp
    time_t ahora = time(NULL);
    struct tm *tm_info = localtime(&ahora);
    char nombre_archivo[100];
    strftime(nombre_archivo, sizeof(nombre_archivo), 
             "reporte_alerta_%Y%m%d_%H%M%S.txt", tm_info);
    
    FILE *archivo = fopen(nombre_archivo, "w");
    
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo de reporte.\n");
        return;
    }
    
    // Cabecera del reporte
    fprintf(archivo, "===========================================================\n");
    fprintf(archivo, "           SISTEMA DE ALERTAS DE CONTAMINACIÓN\n");
    fprintf(archivo, "===========================================================\n");
    fprintf(archivo, "Fecha de generación: %s", asctime(tm_info));
    fprintf(archivo, "Zonas monitoreadas: %d\n", num_zonas);
    fprintf(archivo, "Límites OMS: CO2=%.2f, SO2=%.2f, NO2=%.2f, PM2.5=%.2f\n",
            limites_oms[0], limites_oms[1], limites_oms[2], limites_oms[3]);
    fprintf(archivo, "===========================================================\n\n");
    
    int total_alertas = 0;
    int total_emergencias = 0;
    
    // Reporte por zona
    for (int z = 0; z < num_zonas; z++) {
        fprintf(archivo, "ZONA: %s (ID: %d)\n", zonas[z].nombre, zonas[z].id);
        fprintf(archivo, "Ubicación: Lat %.4f, Lon %.4f\n", 
                zonas[z].latitud, zonas[z].longitud);
        fprintf(archivo, "Población: %d habitantes\n", zonas[z].poblacion);
        fprintf(archivo, "Factor de contaminación: %.2f\n", zonas[z].factor_contaminacion);
        fprintf(archivo, "Última medición: %s\n", 
                zonas[z].historico[DIAS_SIMULACION-1].fecha);
        fprintf(archivo, "-----------------------------------------------------------\n");
        
        fprintf(archivo, "CONTAMINANTES:\n");
        fprintf(archivo, "%-10s %-10s %-10s %-10s %-15s\n", 
                "Tipo", "Actual", "Predic.", "Límite", "Estado");
        fprintf(archivo, "-----------------------------------------------------------\n");
        
        int alertas_zona = 0;
        int emergencias_zona = 0;
        
        for (int c = 0; c < NUM_CONTAMINANTES; c++) {
            float actual = zonas[z].historico[DIAS_SIMULACION-1].valores[c];
            float prediccion = predicciones[z].prediccion_24h[c];
            float limite = limites_oms[c];
            
            char estado_actual[20];
            char estado_prediccion[20];
            
            // Estado actual
            if (actual > limite * 2.0) {
                strcpy(estado_actual, "EMERGENCIA");
                emergencias_zona++;
            } else if (actual > limite * 1.5) {
                strcpy(estado_actual, "ALERTA ALTA");
                alertas_zona++;
            } else if (actual > limite) {
                strcpy(estado_actual, "ALERTA");
                alertas_zona++;
            } else if (actual > limite * 0.8) {
                strcpy(estado_actual, "PRECAUCION");
            } else {
                strcpy(estado_actual, "NORMAL");
            }
            
            // Estado predicción
            if (prediccion > limite * 2.0) {
                strcpy(estado_prediccion, "EMERGENCIA");
            } else if (prediccion > limite * 1.5) {
                strcpy(estado_prediccion, "ALERTA ALTA");
            } else if (prediccion > limite) {
                strcpy(estado_prediccion, "ALERTA");
            } else if (prediccion > limite * 0.8) {
                strcpy(estado_prediccion, "PRECAUCION");
            } else {
                strcpy(estado_prediccion, "NORMAL");
            }
            
            fprintf(archivo, "%-10s %-10.2f %-10.2f %-10.2f %s/%s\n",
                    nombres_contaminantes[c], actual, prediccion, limite,
                    estado_actual, estado_prediccion);
        }
        
        fprintf(archivo, "\nNIVEL DE ALERTA GLOBAL: %d - %s\n",
                predicciones[z].nivel_alerta, predicciones[z].alerta);
        
        if (alertas_zona > 0 || emergencias_zona > 0) {
            fprintf(archivo, "RECOMENDACIONES:\n");
            
            // Recomendaciones basadas en contaminantes críticos
            for (int c = 0; c < NUM_CONTAMINANTES; c++) {
                float actual = zonas[z].historico[DIAS_SIMULACION-1].valores[c];
                
                if (actual > limites_oms[c] * 1.5) {
                    if (c == 0) { // CO2
                        fprintf(archivo, "- Reducir tráfico vehicular en %s\n", zonas[z].nombre);
                    } else if (c == 1) { // SO2
                        fprintf(archivo, "- Limitar operaciones industriales en %s\n", zonas[z].nombre);
                    } else if (c == 2) { // NO2
                        fprintf(archivo, "- Controlar emisiones de vehículos en %s\n", zonas[z].nombre);
                    } else if (c == 3) { // PM2.5
                        fprintf(archivo, "- Suspender actividades al aire libre en %s\n", zonas[z].nombre);
                    }
                }
            }
        }
        
        fprintf(archivo, "===========================================================\n\n");
        
        total_alertas += alertas_zona;
        total_emergencias += emergencias_zona;
    }
    
    // Resumen general
    fprintf(archivo, "RESUMEN GENERAL:\n");
    fprintf(archivo, "Total de zonas: %d\n", num_zonas);
    fprintf(archivo, "Alertas activas: %d\n", total_alertas);
    fprintf(archivo, "Emergencias activas: %d\n", total_emergencias);
    fprintf(archivo, "Porcentaje de zonas con alertas: %.1f%%\n", 
            (total_alertas > 0 ? 100.0 : 0.0));
    
    // Firma del sistema
    fprintf(archivo, "\n===========================================================\n");
    fprintf(archivo, "Sistema Integral de Gestión y Predicción de Contaminación\n");
    fprintf(archivo, "Universidad de las Américas - Programación I\n");
    fprintf(archivo, "===========================================================\n");
    
    fclose(archivo);
    printf("✓ Reporte de alertas guardado en '%s'.\n", nombre_archivo);
}

// ==================== GUARDAR CONFIGURACIÓN ====================

void guardar_configuracion(Zona *zonas, int num_zonas) {
    FILE *archivo = fopen("configuracion_sistema.cfg", "w");
    
    if (archivo == NULL) {
        printf("Error: No se pudo guardar la configuración.\n");
        return;
    }
    
    time_t ahora = time(NULL);
    struct tm *tm_info = localtime(&ahora);
    
    fprintf(archivo, "# Configuración del Sistema de Contaminación\n");
    fprintf(archivo, "# Guardado: %s", asctime(tm_info));
    fprintf(archivo, "NUM_ZONAS=%d\n", num_zonas);
    fprintf(archivo, "DIAS_SIMULACION=%d\n", DIAS_SIMULACION);
    fprintf(archivo, "NUM_CONTAMINANTES=%d\n", NUM_CONTAMINANTES);
    fprintf(archivo, "\n# Límites OMS\n");
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        fprintf(archivo, "LIMITE_%s=%.2f\n", nombres_contaminantes[c], limites_oms[c]);
    }
    
    fprintf(archivo, "\n# Configuración de zonas\n");
    for (int z = 0; z < num_zonas; z++) {
        fprintf(archivo, "\n[ZONA_%d]\n", zonas[z].id);
        fprintf(archivo, "NOMBRE=%s\n", zonas[z].nombre);
        fprintf(archivo, "POBLACION=%d\n", zonas[z].poblacion);
        fprintf(archivo, "LATITUD=%.4f\n", zonas[z].latitud);
        fprintf(archivo, "LONGITUD=%.4f\n", zonas[z].longitud);
        fprintf(archivo, "FACTOR_CONTAMINACION=%.2f\n", zonas[z].factor_contaminacion);
    }
    
    fclose(archivo);
    printf("✓ Configuración guardada en 'configuracion_sistema.cfg'.\n");
}

// ==================== CARGAR CONFIGURACIÓN ====================

void cargar_configuracion(Zona **zonas, int *num_zonas) {
    FILE *archivo = fopen("configuracion_sistema.cfg", "r");
    
    if (archivo == NULL) {
        printf("Archivo de configuración no encontrado.\n");
        return;
    }
    
    char linea[256];
    int zona_actual = -1;
    
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        // Saltar comentarios y líneas vacías
        if (linea[0] == '#' || linea[0] == '\n') {
            continue;
        }
        
        // Quitar salto de línea
        linea[strcspn(linea, "\n")] = '\0';
        
        // Detectar sección de zona
        if (linea[0] == '[' && strstr(linea, "ZONA_") != NULL) {
            sscanf(linea, "[ZONA_%d]", &zona_actual);
            zona_actual--; // Convertir a índice 0-based
        } 
        // Leer límites OMS
        else if (strstr(linea, "LIMITE_CO2") != NULL) {
            sscanf(linea, "LIMITE_CO2=%f", &limites_oms[0]);
        }
        else if (strstr(linea, "LIMITE_SO2") != NULL) {
            sscanf(linea, "LIMITE_SO2=%f", &limites_oms[1]);
        }
        else if (strstr(linea, "LIMITE_NO2") != NULL) {
            sscanf(linea, "LIMITE_NO2=%f", &limites_oms[2]);
        }
        else if (strstr(linea, "LIMITE_PM2.5") != NULL) {
            sscanf(linea, "LIMITE_PM2.5=%f", &limites_oms[3]);
        }
        // Leer configuración de zona
        else if (zona_actual >= 0 && zona_actual < NUM_ZONAS) {
            if (strstr(linea, "NOMBRE=") != NULL) {
                strcpy((*zonas)[zona_actual].nombre, linea + 7);
            }
            else if (strstr(linea, "POBLACION=") != NULL) {
                sscanf(linea + 10, "%d", &(*zonas)[zona_actual].poblacion);
            }
            else if (strstr(linea, "LATITUD=") != NULL) {
                sscanf(linea + 8, "%f", &(*zonas)[zona_actual].latitud);
            }
            else if (strstr(linea, "LONGITUD=") != NULL) {
                sscanf(linea + 9, "%f", &(*zonas)[zona_actual].longitud);
            }
            else if (strstr(linea, "FACTOR_CONTAMINACION=") != NULL) {
                sscanf(linea + 21, "%f", &(*zonas)[zona_actual].factor_contaminacion);
            }
        }
    }
    
    fclose(archivo);
    printf("✓ Configuración cargada desde 'configuracion_sistema.cfg'.\n");
}

// ==================== LEER ARCHIVO HISTÓRICO ====================

void leer_archivo_historico(void) {
    FILE *archivo = fopen(ARCHIVO_HISTORICO, "r");
    
    if (archivo == NULL) {
        printf("No hay registros históricos disponibles.\n");
        return;
    }
    
    printf("\n=== REGISTROS HISTÓRICOS SEMANALES ===\n\n");
    
    char linea[256];
    int contador = 0;
    
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        // Mostrar líneas de contenido, saltar líneas vacías
        if (strlen(linea) > 2) {
            printf("%s", linea);
            contador++;
        }
    }
    
    fclose(archivo);
    
    if (contador == 0) {
        printf("El archivo histórico está vacío.\n");
    } else {
        printf("\nTotal de registros: %d\n", contador / 10); // Aproximado
    }
}

// ==================== LIMPIAR ARCHIVO HISTÓRICO ====================

void limpiar_archivo_historico(void) {
    printf("¿Está seguro de limpiar el archivo histórico? (s/n): ");
    char resp[8];
    leer_cadena("", resp, sizeof(resp));
    if (resp[0] == 's' || resp[0] == 'S') {
        FILE *archivo = fopen(ARCHIVO_HISTORICO, "w");
        if (archivo) { fclose(archivo); printf("✓ Archivo histórico limpiado correctamente.\n"); }
        else { printf("Error: No se pudo limpiar el archivo histórico.\n"); }
    } else {
        printf("Operación cancelada.\n");
    }
}

// ==================== EXPORTAR A JSON ====================

void exportar_a_json(Zona *zonas, int num_zonas, const char *nombre_archivo) {
    if (zonas == NULL || num_zonas <= 0) {
        printf("Error: Datos inválidos para exportar JSON.\n");
        return;
    }
    
    char nombre_completo[150];
    if (strstr(nombre_archivo, ".json") == NULL) {
        snprintf(nombre_completo, sizeof(nombre_completo), "%s.json", nombre_archivo);
    } else {
        strncpy(nombre_completo, nombre_archivo, sizeof(nombre_completo) - 1);
    }
    
    FILE *archivo = fopen(nombre_completo, "w");
    
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo JSON.\n");
        return;
    }
    
    time_t ahora = time(NULL);
    struct tm *tm_info = localtime(&ahora);
    char fecha_json[30];
    strftime(fecha_json, 30, "%Y-%m-%dT%H:%M:%S", tm_info);
    
    // Encabezado JSON
    fprintf(archivo, "{\n");
    fprintf(archivo, "  \"sistema_contaminacion\": {\n");
    fprintf(archivo, "    \"fecha_generacion\": \"%s\",\n", fecha_json);
    fprintf(archivo, "    \"zonas_monitoreadas\": %d,\n", num_zonas);
    fprintf(archivo, "    \"dias_simulacion\": %d,\n", DIAS_SIMULACION);
    fprintf(archivo, "    \"limites_oms\": {\n");
    fprintf(archivo, "      \"CO2\": %.2f,\n", limites_oms[0]);
    fprintf(archivo, "      \"SO2\": %.2f,\n", limites_oms[1]);
    fprintf(archivo, "      \"NO2\": %.2f,\n", limites_oms[2]);
    fprintf(archivo, "      \"PM2_5\": %.2f\n", limites_oms[3]);
    fprintf(archivo, "    },\n");
    fprintf(archivo, "    \"datos_zonas\": [\n");
    
    // Datos por zona
    for (int z = 0; z < num_zonas; z++) {
        fprintf(archivo, "      {\n");
        fprintf(archivo, "        \"id\": %d,\n", zonas[z].id);
        fprintf(archivo, "        \"nombre\": \"%s\",\n", zonas[z].nombre);
        fprintf(archivo, "        \"poblacion\": %d,\n", zonas[z].poblacion);
        fprintf(archivo, "        \"ubicacion\": {\n");
        fprintf(archivo, "          \"latitud\": %.4f,\n", zonas[z].latitud);
        fprintf(archivo, "          \"longitud\": %.4f\n", zonas[z].longitud);
        fprintf(archivo, "        },\n");
        fprintf(archivo, "        \"factor_contaminacion\": %.2f,\n", zonas[z].factor_contaminacion);
        fprintf(archivo, "        \"registros_diarios\": [\n");
        
        // Registros diarios
        for (int d = 0; d < DIAS_SIMULACION; d++) {
            fprintf(archivo, "          {\n");
            fprintf(archivo, "            \"fecha\": \"%s\",\n", zonas[z].historico[d].fecha);
            fprintf(archivo, "            \"contaminantes\": {\n");
            
            for (int c = 0; c < NUM_CONTAMINANTES; c++) {
                fprintf(archivo, "              \"%s\": {\n", nombres_contaminantes[c]);
                fprintf(archivo, "                \"valor\": %.2f,\n", zonas[z].historico[d].valores[c]);
                fprintf(archivo, "                \"tendencia\": %d,\n", zonas[z].historico[d].tendencia[c]);
                fprintf(archivo, "                \"dias_consecutivos\": %d\n", 
                        zonas[z].historico[d].dias_consecutivos[c]);
                
                if (c < NUM_CONTAMINANTES - 1) {
                    fprintf(archivo, "              },\n");
                } else {
                    fprintf(archivo, "              }\n");
                }
            }
            
            fprintf(archivo, "            }\n");
            
            if (d < DIAS_SIMULACION - 1) {
                fprintf(archivo, "          },\n");
            } else {
                fprintf(archivo, "          }\n");
            }
        }
        
        fprintf(archivo, "        ]\n");
        
        if (z < num_zonas - 1) {
            fprintf(archivo, "      },\n");
        } else {
            fprintf(archivo, "      }\n");
        }
    }
    
    // Cierre JSON
    fprintf(archivo, "    ]\n");
    fprintf(archivo, "  }\n");
    fprintf(archivo, "}\n");
    
    fclose(archivo);
    printf("✓ Datos exportados a JSON en '%s'.\n", nombre_completo);
}