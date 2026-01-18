#include "contaminacion.h"

// Variables globales
char *nombres_contaminantes[NUM_CONTAMINANTES] = {"CO2", "SO2", "NO2", "PM2.5"};
float limites_oms[NUM_CONTAMINANTES] = {50.0, 20.0, 25.0, 15.0};

// Nombres de zonas predefinidas
char *nombres_zonas[NUM_ZONAS] = {
    "Centro Urbano", 
    "Zona Industrial", 
    "Residencial Norte", 
    "Residencial Sur", 
    "Area Comercial"
};

// ==================== FUNCIONES DE ENTRADA/SAIDA SEGURAS ====================

void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int leer_entero(char *mensaje) {
    char entrada[64];
    int intentos = 0;
    while (intentos < 3) {
        printf("%s", mensaje);
        fflush(stdout);
        if (!fgets(entrada, sizeof(entrada), stdin)) return 0;
        entrada[strcspn(entrada, "\r\n")] = '\0';
        if (entrada[0] == '\0') { intentos++; continue; }
        char *endptr = NULL;
        long val = strtol(entrada, &endptr, 10);
        if (endptr != entrada && *endptr == '\0') return (int)val;
        printf("'%s' no es un número entero válido. Intento %d/3\n", entrada, intentos + 1);
        intentos++;
    }
    return 0;
}

float leer_float(char *mensaje) {
    char entrada[64];
    int intentos = 0;
    while (intentos < 3) {
        printf("%s", mensaje);
        fflush(stdout);
        if (!fgets(entrada, sizeof(entrada), stdin)) return 0.0f;
        entrada[strcspn(entrada, "\r\n")] = '\0';
        if (entrada[0] == '\0') { intentos++; continue; }
        char *endptr = NULL;
        float val = strtof(entrada, &endptr);
        if (endptr != entrada && *endptr == '\0') return val;
        printf("'%s' no es un número decimal válido. Intento %d/3\n", entrada, intentos + 1);
        intentos++;
    }
    return 0.0f;
}

void leer_cadena(char *mensaje, char *buffer, int tamanio) {
    printf("%s", mensaje);
    fflush(stdout);
    if (!fgets(buffer, tamanio, stdin)) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';
}

// ==================== FUNCIONES DE INICIALIZACIÓN ====================

void inicializar_sistema(Zona **zonas, int *num_zonas) {
    srand((unsigned int)time(NULL));
    
    *num_zonas = NUM_ZONAS;
    *zonas = (Zona *)malloc(NUM_ZONAS * sizeof(Zona));
    
    if (*zonas == NULL) {
        fprintf(stderr, "Error crítico: No se pudo asignar memoria para las zonas.\n");
        exit(EXIT_FAILURE);
    }
    
    // Datos predefinidos para cada zona
    int poblaciones[NUM_ZONAS] = {50000, 20000, 35000, 40000, 45000};
    float latitudes[NUM_ZONAS] = {10.4910, 10.5010, 10.5110, 10.5210, 10.5310};
    float longitudes[NUM_ZONAS] = {-66.9020, -66.9120, -66.9220, -66.9320, -66.9420};
    float factores_contaminacion[NUM_ZONAS] = {1.5, 2.0, 1.0, 1.2, 1.8};
    
    // Obtener fecha actual para simulación
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    struct tm fecha_base = *tm_info; // Copia para no modificar la original
    
    for (int z = 0; z < NUM_ZONAS; z++) {
        Zona *zona_actual = &((*zonas)[z]);
        
        zona_actual->id = z + 1;
        strncpy(zona_actual->nombre, nombres_zonas[z], sizeof(zona_actual->nombre) - 1);
        zona_actual->nombre[sizeof(zona_actual->nombre) - 1] = '\0';
        
        zona_actual->poblacion = poblaciones[z];
        zona_actual->latitud = latitudes[z];
        zona_actual->longitud = longitudes[z];
        zona_actual->factor_contaminacion = factores_contaminacion[z];
        
        // Inicializar valores iniciales aleatorios para cada contaminante
        for (int c = 0; c < NUM_CONTAMINANTES; c++) {
            zona_actual->historico[0].valores[c] = (float)((rand() % 71) + 20); // 20-90
            zona_actual->historico[0].tendencia[c] = 0;
            zona_actual->historico[0].dias_consecutivos[c] = 0;
        }
        
        // Establecer fecha inicial (hoy - 30 días)
        struct tm fecha_inicio = fecha_base;
        fecha_inicio.tm_mday -= DIAS_SIMULACION - 1;
        mktime(&fecha_inicio);
        strftime(zona_actual->historico[0].fecha, sizeof(zona_actual->historico[0].fecha), 
                "%Y-%m-%d", &fecha_inicio);
    }
    
    printf("Sistema inicializado con %d zonas urbanas.\n", NUM_ZONAS);
}

void liberar_memoria(Zona **zonas, int num_zonas) {
    if (zonas != NULL && *zonas != NULL) {
        free(*zonas);
        *zonas = NULL;
        printf("Memoria liberada correctamente.\n");
    }
}

// ==================== FUNCIONES DE CLIMA ====================

Clima generar_clima_aleatorio(void) {
    Clima clima;
    clima.temperatura = (float)((rand() % 30) + 15); // 15-45°C
    clima.velocidad_viento = (float)((rand() % 60) / 10.0); // 0-6 m/s
    clima.humedad = (float)((rand() % 80) + 20); // 20-100%
    clima.presion_atmosferica = (float)((rand() % 50) + 990); // 990-1040 hPa
    return clima;
}

// ==================== FUNCIONES DE PROBABILIDAD ====================

ProbabilidadCambio calcular_probabilidades(Zona *zona, int contaminante, 
                                          int dia_anterior, Clima clima) {
    ProbabilidadCambio prob;
    
    // Probabilidad base (50% subir, 50% bajar)
    prob.base = 50.0;
    
    // Ajuste por tendencia actual
    int tendencia = zona->historico[dia_anterior].tendencia[contaminante];
    int dias_consec = zona->historico[dia_anterior].dias_consecutivos[contaminante];
    
    if (tendencia > 0) { // Si está subiendo
        // Mientras más días subiendo, menos probabilidad de seguir subiendo
        prob.ajuste_tendencia = -8.0 * (float)dias_consec;
        
        // Límite máximo de reducción
        if (prob.ajuste_tendencia < -40.0) {
            prob.ajuste_tendencia = -40.0;
        }
    } else if (tendencia < 0) { // Si está bajando
        // Mientras más días bajando, menos probabilidad de seguir bajando
        prob.ajuste_tendencia = 8.0 * (float)dias_consec;
        
        // Límite máximo de aumento
        if (prob.ajuste_tendencia > 40.0) {
            prob.ajuste_tendencia = 40.0;
        }
    } else {
        prob.ajuste_tendencia = 0.0;
    }
    
    // Ajuste por condiciones climáticas
    prob.ajuste_clima = 0.0;
    
    // Temperatura: más calor = más probabilidad de subir
    prob.ajuste_clima += (clima.temperatura - 25.0) * 1.2;
    
    // Viento: más viento = menos probabilidad de subir (dispersión)
    prob.ajuste_clima -= clima.velocidad_viento * 3.0;
    
    // Humedad alta puede aumentar algunos contaminantes
    if (contaminante == 3) { // PM2.5 afectado por humedad
        prob.ajuste_clima += (clima.humedad - 60.0) * 0.3;
    }
    
    // Presión baja = menos dispersión = más probabilidad de subir
    prob.ajuste_clima += (1013.0 - clima.presion_atmosferica) * 0.2;
    
    // Ajuste por tipo de zona
    prob.ajuste_zona = (zona->factor_contaminacion - 1.0) * 15.0;
    
    return prob;
}

float calcular_magnitud_cambio(Zona *zona, int contaminante, 
                              int dia_anterior, Clima clima) {
    // Magnitud base entre 1 y 5 puntos
    float magnitud_base = (float)((rand() % 5) + 1);
    
    // Ajustar por nivel actual respecto al límite OMS
    float valor_actual = zona->historico[dia_anterior].valores[contaminante];
    float porcentaje_limite = (valor_actual / limites_oms[contaminante]) * 100.0;
    
    float factor_ajuste = 1.0;
    
    // Si está muy cerca o sobre el límite, cambios más conservadores
    if (porcentaje_limite > 150.0) {
        factor_ajuste *= 0.7; // Reducir cambios cuando está muy alto
    } else if (porcentaje_limite > 120.0) {
        factor_ajuste *= 0.85;
    } else if (porcentaje_limite < 50.0) {
        factor_ajuste *= 1.3; // Aumentar cambios cuando está muy bajo
    }
    
    // Ajustar por clima extremo
    if (clima.temperatura > 35.0) {
        factor_ajuste *= 1.2; // Calor extremo = cambios más grandes
    }
    
    if (clima.velocidad_viento < 1.0) {
        factor_ajuste *= 1.3; // Poco viento = cambios más grandes
    }
    
    // Ajustar por factor de zona
    factor_ajuste *= zona->factor_contaminacion;
    
    // Límites para el factor de ajuste
    if (factor_ajuste < 0.5) factor_ajuste = 0.5;
    if (factor_ajuste > 2.0) factor_ajuste = 2.0;
    
    return magnitud_base * factor_ajuste;
}

// ==================== FUNCIONES DE SIMULACIÓN ====================

void simular_datos(Zona *zona, int dia_actual, Clima clima) {
    if (dia_actual <= 0 || dia_actual >= DIAS_SIMULACION) {
        fprintf(stderr, "Error: Día %d fuera de rango válido.\n", dia_actual);
        return;
    }
    
    int dia_anterior = dia_actual - 1;
    
    // Contar contaminantes con decrecimiento prolongado
    int contaminantes_decrecientes[NUM_CONTAMINANTES];
    int num_decrecientes = 0;
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        if (zona->historico[dia_anterior].tendencia[c] < 0 && 
            zona->historico[dia_anterior].dias_consecutivos[c] >= UMBRAL_DECRECIMIENTO) {
            contaminantes_decrecientes[num_decrecientes++] = c;
        }
    }
    
    // Generar nuevos valores para cada contaminante
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        // 1. Calcular probabilidades de cambio
        ProbabilidadCambio prob = calcular_probabilidades(zona, c, dia_anterior, clima);
        
        // 2. Calcular probabilidad total de subir
        float prob_subir = prob.base + prob.ajuste_tendencia + 
                          prob.ajuste_clima + prob.ajuste_zona;
        
        // 3. Efecto de otros contaminantes decrecientes
        if (num_decrecientes > 0 && num_decrecientes < NUM_CONTAMINANTES) {
            // Si hay contaminantes decreciendo, aumenta probabilidad de que otros suban
            prob_subir += 12.0 * (float)num_decrecientes;
        }
        
        // 4. Limitar probabilidad entre 10% y 90%
        if (prob_subir < 10.0) prob_subir = 10.0;
        if (prob_subir > 90.0) prob_subir = 90.0;
        
        // 5. Determinar dirección del cambio basado en probabilidad
        int direccion;
        int random_val = rand() % 100;
        
        if (random_val < (int)prob_subir) {
            direccion = 1; // Subir
        } else {
            direccion = -1; // Bajar
        }
        
        // 6. Calcular magnitud del cambio (1-5 puntos base, ajustada)
        float magnitud = calcular_magnitud_cambio(zona, c, dia_anterior, clima);
        
        // 7. Aplicar cambio con dirección
        float cambio = direccion * magnitud;
        
        // 8. Aplicar el cambio al valor anterior
        float nuevo_valor = zona->historico[dia_anterior].valores[c] + cambio;
        
        // 9. Limitar valores entre 0 y 200
        if (nuevo_valor < 0.0) {
            nuevo_valor = 0.0;
        } else if (nuevo_valor > 200.0) {
            nuevo_valor = 200.0;
        }
        
        zona->historico[dia_actual].valores[c] = nuevo_valor;
        
        // 10. Actualizar tendencia y días consecutivos
        if (cambio > 0.3) { // Subió significativamente
            zona->historico[dia_actual].tendencia[c] = 1;
            if (zona->historico[dia_anterior].tendencia[c] > 0) {
                zona->historico[dia_actual].dias_consecutivos[c] = 
                    zona->historico[dia_anterior].dias_consecutivos[c] + 1;
            } else {
                zona->historico[dia_actual].dias_consecutivos[c] = 1;
            }
        } else if (cambio < -0.3) { // Bajó significativamente
            zona->historico[dia_actual].tendencia[c] = -1;
            if (zona->historico[dia_anterior].tendencia[c] < 0) {
                zona->historico[dia_actual].dias_consecutivos[c] = 
                    zona->historico[dia_anterior].dias_consecutivos[c] + 1;
            } else {
                zona->historico[dia_actual].dias_consecutivos[c] = 1;
            }
        } else { // Cambio mínimo, se considera estable
            zona->historico[dia_actual].tendencia[c] = 0;
            zona->historico[dia_actual].dias_consecutivos[c] = 0;
        }
    }
    
    // Actualizar fecha para el nuevo día
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    struct tm fecha_nueva = *tm_info; // Copia
    
    fecha_nueva.tm_mday -= (DIAS_SIMULACION - dia_actual - 1);
    mktime(&fecha_nueva);
    
    strftime(zona->historico[dia_actual].fecha, 
             sizeof(zona->historico[dia_actual].fecha), 
             "%Y-%m-%d", &fecha_nueva);
    
    // Crear copia histórica cada semana
    if (dia_actual % DIAS_PARA_COPIA == 0 && dia_actual > 0) {
        guardar_copia_historica(zona, dia_actual);
    }
}

// ==================== FUNCIONES DE PERSISTENCIA ====================

void guardar_copia_historica(Zona *zona, int dia_actual) {
    FILE *archivo = fopen(ARCHIVO_HISTORICO, "ab");
    
    if (archivo == NULL) {
        fprintf(stderr, "Advertencia: No se pudo abrir archivo histórico para escritura.\n");
        return;
    }
    
    // Escribir registro de copia
    fprintf(archivo, "=== COPIA HISTORICA SEMANAL ===\n");
    fprintf(archivo, "Zona: %s\n", zona->nombre);
    fprintf(archivo, "Día de simulación: %d\n", dia_actual + 1);
    fprintf(archivo, "Fecha correspondiente: %s\n", zona->historico[dia_actual].fecha);
    fprintf(archivo, "Valores de contaminantes:\n");
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        fprintf(archivo, "  %s: %.2f ", nombres_contaminantes[c], 
                zona->historico[dia_actual].valores[c]);
        
        if (zona->historico[dia_actual].tendencia[c] > 0) {
            fprintf(archivo, "(↑ SUBIENDO, %d días consecutivos)", 
                    zona->historico[dia_actual].dias_consecutivos[c]);
        } else if (zona->historico[dia_actual].tendencia[c] < 0) {
            fprintf(archivo, "(↓ BAJANDO, %d días consecutivos)", 
                    zona->historico[dia_actual].dias_consecutivos[c]);
        } else {
            fprintf(archivo, "(→ ESTABLE)");
        }
        
        fprintf(archivo, "\n");
    }
    
    fprintf(archivo, "Factor de zona: %.2f\n", zona->factor_contaminacion);
    fprintf(archivo, "=================================\n\n");
    
    fclose(archivo);
}

// ==================== FUNCIONES DE VISUALIZACIÓN ====================

void mostrar_promedios(Zona *zona) {
    if (zona == NULL) {
        printf("Error: Zona no válida.\n");
        return;
    }
    
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║       PROMEDIOS HISTÓRICOS (30 DÍAS)          ║\n");
    printf("║               %-30s ║\n", zona->nombre);
    printf("╠════════════════════════════════════════════════╣\n");
    printf("║ %-12s %-8s %-8s %-10s %-8s ║\n", 
           "Contaminante", "Promedio", "Límite", "% Límite", "Estado");
    printf("╠════════════════════════════════════════════════╣\n");
    
    float min_global = 9999, max_global = 0;
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        float suma = 0.0;
        float minimo = 9999.0, maximo = 0.0;
        
        for (int d = 0; d < DIAS_SIMULACION; d++) {
            float valor = zona->historico[d].valores[c];
            suma += valor;
            
            if (valor < minimo) minimo = valor;
            if (valor > maximo) maximo = valor;
        }
        
        float promedio = suma / DIAS_SIMULACION;
        float porcentaje = (promedio / limites_oms[c]) * 100.0;
        
        // Actualizar min/max global
        if (minimo < min_global) min_global = minimo;
        if (maximo > max_global) max_global = maximo;
        
        // Determinar estado
        char estado[15];
        char simbolo[5];
        
        if (porcentaje > 150.0) {
            strcpy(estado, "CRÍTICO");
            strcpy(simbolo, "🔥");
        } else if (porcentaje > 120.0) {
            strcpy(estado, "ALTO");
            strcpy(simbolo, "⚠️");
        } else if (porcentaje > 100.0) {
            strcpy(estado, "ELEVADO");
            strcpy(simbolo, "⬆️");
        } else if (porcentaje > 80.0) {
            strcpy(estado, "MODERADO");
            strcpy(simbolo, "➡️");
        } else {
            strcpy(estado, "ACEPTABLE");
            strcpy(simbolo, "✅");
        }
        
        printf("║ %-12s %-8.2f %-8.2f %-10.1f%% %-8s %s ║\n", 
               nombres_contaminantes[c], promedio, limites_oms[c], 
               porcentaje, estado, simbolo);
    }
    
    printf("╠════════════════════════════════════════════════╣\n");
    printf("║ Rango histórico: Min=%-6.2f Max=%-6.2f        ║\n", 
           min_global, max_global);
    printf("║ Factor de zona: %-6.2f                        ║\n", 
           zona->factor_contaminacion);
    printf("╚════════════════════════════════════════════════╝\n");
}

void mostrar_detalle_zona(Zona *zona) {
    if (zona == NULL) {
        printf("Error: Zona no válida.\n");
        return;
    }
    
    int ultimo_dia = DIAS_SIMULACION - 1;
    
    printf("\n┌────────────────────────────────────────────────┐\n");
    printf("│          DETALLE DE CONTAMINACIÓN             │\n");
    printf("├────────────────────────────────────────────────┤\n");
    printf("│ Zona: %-40s │\n", zona->nombre);
    printf("│ ID: %-3d  Población: %-8d habitantes     │\n", 
           zona->id, zona->poblacion);
    printf("│ Ubicación: Lat %.4f, Lon %.4f           │\n", 
           zona->latitud, zona->longitud);
    printf("│ Factor de contaminación: %-5.2f                │\n", 
           zona->factor_contaminacion);
    printf("├────────────────────────────────────────────────┤\n");
    printf("│ Última medición: %-29s │\n", zona->historico[ultimo_dia].fecha);
    printf("├────────────────────────────────────────────────┤\n");
    
    int alertas = 0;
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        float valor = zona->historico[ultimo_dia].valores[c];
        int tendencia = zona->historico[ultimo_dia].tendencia[c];
        int dias_consec = zona->historico[ultimo_dia].dias_consecutivos[c];
        
        char tendencia_simbolo = '→';
        char tendencia_texto[15];
        
        if (tendencia > 0) {
            tendencia_simbolo = '↑';
            snprintf(tendencia_texto, sizeof(tendencia_texto), "SUBIENDO (%d)", dias_consec);
        } else if (tendencia < 0) {
            tendencia_simbolo = '↓';
            snprintf(tendencia_texto, sizeof(tendencia_texto), "BAJANDO (%d)", dias_consec);
        } else {
            strcpy(tendencia_texto, "ESTABLE");
        }
        
        printf("│ %-5s: %6.2f %c %-20s", 
               nombres_contaminantes[c], valor, tendencia_simbolo, tendencia_texto);
        
        if (valor > limites_oms[c]) {
            float exceso = ((valor / limites_oms[c]) - 1.0) * 100.0;
            printf(" +%.1f%%", exceso);
            alertas++;
        }
        printf(" │\n");
    }
    
    printf("├────────────────────────────────────────────────┤\n");
    
    if (alertas > 0) {
        printf("│ ALERTAS ACTIVAS: %-2d contaminante(s) excede(n)│\n", alertas);
        printf("│ los límites de la OMS.                      │\n");
    } else {
        printf("│ Todos los contaminantes dentro de límites  │\n");
        printf("│ aceptables de la OMS.                      │\n");
    }
    
    printf("└────────────────────────────────────────────────┘\n");
}

void mostrar_tendencias_detalladas(Zona *zona) {
    if (zona == NULL) {
        printf("Error: Zona no válida.\n");
        return;
    }
    
    int ultimo_dia = DIAS_SIMULACION - 1;
    
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│               TENDENCIAS DETALLADAS                       │\n");
    printf("│                    %-35s │\n", zona->nombre);
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│ Contaminante │ Últ.Valor │ Tendencia  │ Días cons. │ Prob.│\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        float valor = zona->historico[ultimo_dia].valores[c];
        int tendencia = zona->historico[ultimo_dia].tendencia[c];
        int dias_consec = zona->historico[ultimo_dia].dias_consecutivos[c];
        
        // Calcular probabilidad estimada de cambio
        float probabilidad;
        if (tendencia > 0) {
            probabilidad = 60.0 - (dias_consec * 4.0);
            if (probabilidad < 20.0) probabilidad = 20.0;
        } else if (tendencia < 0) {
            probabilidad = 40.0 + (dias_consec * 4.0);
            if (probabilidad > 80.0) probabilidad = 80.0;
        } else {
            probabilidad = 50.0;
        }
        
        char tendencia_str[15];
        if (tendencia > 0) {
            snprintf(tendencia_str, sizeof(tendencia_str), "↑ %d días", dias_consec);
        } else if (tendencia < 0) {
            snprintf(tendencia_str, sizeof(tendencia_str), "↓ %d días", dias_consec);
        } else {
            strcpy(tendencia_str, "→ Estable");
        }
        
        printf("│ %-12s │ %9.2f │ %-10s │ %10d │ %4.0f%%│\n",
               nombres_contaminantes[c], valor, tendencia_str, dias_consec, probabilidad);
    }
    
    printf("└─────────────────────────────────────────────────────────────┘\n");
}