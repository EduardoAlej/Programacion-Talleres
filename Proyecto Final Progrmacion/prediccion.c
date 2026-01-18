#include "prediccion.h"

// ==================== FUNCIONES DE PREDICCIÓN ====================

float calcular_prediccion(Zona *zona, int contaminante, Clima clima) {
    if (zona == NULL || contaminante < 0 || contaminante >= NUM_CONTAMINANTES) {
        return 0.0;
    }
    
    // 1. Obtener datos históricos de los últimos 7 días
    float valores_recientes[7];
    int dias_disponibles = 0;
    
    for (int i = 0; i < 7 && i < DIAS_SIMULACION; i++) {
        int indice = DIAS_SIMULACION - 1 - i;
        valores_recientes[i] = zona->historico[indice].valores[contaminante];
        dias_disponibles++;
    }
    
    // 2. Calcular promedio ponderado (más peso a días recientes)
    float suma_ponderada = 0.0;
    float suma_pesos = 0.0;
    
    for (int i = 0; i < dias_disponibles; i++) {
        float peso = (7.0 - i) * 0.5; // Peso decreciente: 3.5, 3.0, 2.5, ...
        suma_ponderada += valores_recientes[i] * peso;
        suma_pesos += peso;
    }
    
    float prediccion_base = (suma_pesos > 0) ? suma_ponderada / suma_pesos : valores_recientes[0];
    
    // 3. Ajustar por tendencia actual
    int tendencia_actual = zona->historico[DIAS_SIMULACION-1].tendencia[contaminante];
    int dias_tendencia = zona->historico[DIAS_SIMULACION-1].dias_consecutivos[contaminante];
    
    float ajuste_tendencia = 0.0;
    
    if (tendencia_actual != 0) {
        // Fuerza de la tendencia (más días = más fuerte, pero con límite)
        float fuerza_tendencia = 1.0 + (dias_tendencia * 0.2);
        if (fuerza_tendencia > 3.0) fuerza_tendencia = 3.0;
        
        ajuste_tendencia = tendencia_actual * fuerza_tendencia;
        
        // Si lleva muchos días con la misma tendencia, reducir efecto (se estabiliza)
        if (dias_tendencia > UMBRAL_CRECIMIENTO) {
            ajuste_tendencia *= 0.7;
        }
    }
    
    // 4. Ajustar por condiciones climáticas
    float ajuste_clima = 0.0;
    
    // Temperatura: cada grado sobre 25°C aumenta contaminación
    if (clima.temperatura > 25.0) {
        ajuste_clima += (clima.temperatura - 25.0) * 0.3;
    }
    
    // Viento: mayor velocidad dispersa contaminantes
    ajuste_clima -= clima.velocidad_viento * 0.8;
    
    // Humedad: afecta especialmente a PM2.5
    if (contaminante == 3) { // PM2.5
        if (clima.humedad > 70.0) {
            ajuste_clima += (clima.humedad - 70.0) * 0.1;
        }
    }
    
    // Presión atmosférica: baja presión = menor dispersión
    ajuste_clima += (1013.0 - clima.presion_atmosferica) * 0.05;
    
    // 5. Ajustar por factor de zona
    float ajuste_zona = (zona->factor_contaminacion - 1.0) * 2.0;
    
    // 6. Calcular predicción final
    float prediccion = prediccion_base + ajuste_tendencia + ajuste_clima + ajuste_zona;
    
    // 7. Aplicar límites razonables
    float valor_actual = zona->historico[DIAS_SIMULACION-1].valores[contaminante];
    float limite_inferior = valor_actual * 0.7; // No bajar más del 30%
    float limite_superior = valor_actual * 1.5; // No subir más del 50%
    
    if (prediccion < limite_inferior) prediccion = limite_inferior;
    if (prediccion > limite_superior) prediccion = limite_superior;
    
    // 8. No permitir valores negativos
    if (prediccion < 0.0) prediccion = 0.0;
    
    return prediccion;
}

void calcular_predicciones_zona(Zona *zona, Clima clima, float predicciones[NUM_CONTAMINANTES]) {
    if (zona == NULL || predicciones == NULL) {
        return;
    }
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        predicciones[c] = calcular_prediccion(zona, c, clima);
    }
}

float calcular_factor_riesgo(Zona *zona) {
    if (zona == NULL) {
        return 0.0;
    }
    
    float factor_riesgo = 0.0;
    int contaminantes_criticos = 0;
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        float valor_actual = zona->historico[DIAS_SIMULACION-1].valores[c];
        float porcentaje_limite = (valor_actual / limites_oms[c]) * 100.0;
        
        // Contribución al factor de riesgo
        if (porcentaje_limite > 150.0) {
            factor_riesgo += 2.0;
            contaminantes_criticos++;
        } else if (porcentaje_limite > 120.0) {
            factor_riesgo += 1.5;
            contaminantes_criticos++;
        } else if (porcentaje_limite > 100.0) {
            factor_riesgo += 1.0;
        } else if (porcentaje_limite > 80.0) {
            factor_riesgo += 0.5;
        }
        
        // Considerar tendencias peligrosas
        if (zona->historico[DIAS_SIMULACION-1].tendencia[c] > 0) {
            int dias_subiendo = zona->historico[DIAS_SIMULACION-1].dias_consecutivos[c];
            if (dias_subiendo > 3) {
                factor_riesgo += 0.1 * dias_subiendo;
            }
        }
    }
    
    // Ajustar por número de contaminantes críticos
    if (contaminantes_criticos > 0) {
        factor_riesgo *= (1.0 + (contaminantes_criticos * 0.2));
    }
    
    // Ajustar por factor de zona
    factor_riesgo *= zona->factor_contaminacion;
    
    // Normalizar a escala 0-10
    if (factor_riesgo > 10.0) factor_riesgo = 10.0;
    
    return factor_riesgo;
}

void obtener_recomendaciones(Zona *zona, float predicciones[NUM_CONTAMINANTES], 
                            char recomendaciones[5][200], int *num_recomendaciones) {
    if (zona == NULL || predicciones == NULL || recomendaciones == NULL || num_recomendaciones == NULL) {
        *num_recomendaciones = 0;
        return;
    }
    
    *num_recomendaciones = 0;
    
    // Analizar cada contaminante para generar recomendaciones
    for (int c = 0; c < NUM_CONTAMINANTES && *num_recomendaciones < 5; c++) {
        float valor_actual = zona->historico[DIAS_SIMULACION-1].valores[c];
        float valor_predicho = predicciones[c];
        float limite = limites_oms[c];
        
        // Verificar si requiere acción
        if (valor_actual > limite * 1.3 || valor_predicho > limite * 1.3) {
            if (c == 0) { // CO2
                snprintf(recomendaciones[*num_recomendaciones], 200,
                        "Reducir tráfico vehicular en %s. Considerar implementar pico y placa.",
                        zona->nombre);
                (*num_recomendaciones)++;
            }
            else if (c == 1) { // SO2
                snprintf(recomendaciones[*num_recomendaciones], 200,
                        "Limitar operaciones industriales en %s. Revisar sistemas de filtrado.",
                        zona->nombre);
                (*num_recomendaciones)++;
            }
            else if (c == 2) { // NO2
                snprintf(recomendaciones[*num_recomendaciones], 200,
                        "Controlar emisiones de transporte público en %s. Promover transporte limpio.",
                        zona->nombre);
                (*num_recomendaciones)++;
            }
            else if (c == 3) { // PM2.5
                snprintf(recomendaciones[*num_recomendaciones], 200,
                        "Suspender actividades al aire libre en %s. Usar mascarillas N95.",
                        zona->nombre);
                (*num_recomendaciones)++;
                
                // Recomendación adicional para grupos sensibles
                if (*num_recomendaciones < 5) {
                    snprintf(recomendaciones[*num_recomendaciones], 200,
                            "Proteger grupos sensibles (niños, ancianos, enfermos) en %s.",
                            zona->nombre);
                    (*num_recomendaciones)++;
                }
            }
        }
    }
    
    // Recomendaciones generales si hay múltiples contaminantes elevados
    int contaminantes_elevados = 0;
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        if (zona->historico[DIAS_SIMULACION-1].valores[c] > limites_oms[c]) {
            contaminantes_elevados++;
        }
    }
    
    if (contaminantes_elevados >= 2 && *num_recomendaciones < 5) {
        snprintf(recomendaciones[*num_recomendaciones], 200,
                "Implementar medidas integrales en %s: reducir actividades industriales y vehiculares simultáneamente.",
                zona->nombre);
        (*num_recomendaciones)++;
    }
    
    // Si no hay recomendaciones específicas, agregar una general
    if (*num_recomendaciones == 0 && *num_recomendaciones < 5) {
        snprintf(recomendaciones[*num_recomendaciones], 200,
                "Niveles de contaminación en %s dentro de límites aceptables. Mantener monitoreo.",
                zona->nombre);
        (*num_recomendaciones)++;
    }
}

float calcular_indice_calidad_aire(Zona *zona) {
    if (zona == NULL) {
        return 0.0;
    }
    
    float subindices[NUM_CONTAMINANTES];
    float aqi = 0.0;
    
    // Calcular subíndice para cada contaminante
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        float concentracion = zona->historico[DIAS_SIMULACION-1].valores[c];
        float limite = limites_oms[c];
        
        // Calcular subíndice (0-500 scale, similar a US EPA AQI)
        float subindice = (concentracion / limite) * 100.0;
        
        // Ajustar escala
        if (subindice <= 50.0) {
            subindice = subindice; // Buena
        } else if (subindice <= 100.0) {
            subindice = 50.0 + (subindice - 50.0) * 1.5; // Moderada
        } else if (subindice <= 150.0) {
            subindice = 100.0 + (subindice - 100.0) * 2.0; // Dañina a grupos sensibles
        } else if (subindice <= 200.0) {
            subindice = 150.0 + (subindice - 150.0) * 2.5; // Dañina
        } else if (subindice <= 300.0) {
            subindice = 200.0 + (subindice - 200.0) * 3.0; // Muy dañina
        } else {
            subindice = 300.0 + (subindice - 300.0) * 4.0; // Peligrosa
        }
        
        subindices[c] = subindice;
    }
    
    // El AQI es el máximo de los subíndices
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        if (subindices[c] > aqi) {
            aqi = subindices[c];
        }
    }
    
    // Limitar a 500 (máximo del índice)
    if (aqi > 500.0) aqi = 500.0;
    
    return aqi;
}

int requiere_accion_inmediata(Zona *zona, float predicciones[NUM_CONTAMINANTES]) {
    if (zona == NULL || predicciones == NULL) {
        return 0;
    }
    
    // Verificar condiciones de emergencia
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        float valor_actual = zona->historico[DIAS_SIMULACION-1].valores[c];
        float valor_predicho = predicciones[c];
        float limite = limites_oms[c];
        
        // Acción inmediata si:
        // 1. Valor actual excede 200% del límite
        // 2. Valor predicho excede 200% del límite
        // 3. Tendencia creciente fuerte y valor ya sobre límite
        if (valor_actual > limite * 2.0 || valor_predicho > limite * 2.0) {
            return 1; // Acción inmediata requerida
        }
        
        // Verificar tendencia peligrosa
        if (zona->historico[DIAS_SIMULACION-1].tendencia[c] > 0 &&
            zona->historico[DIAS_SIMULACION-1].dias_consecutivos[c] >= 5 &&
            valor_actual > limite) {
            return 1; // Acción inmediata requerida
        }
    }
    
    // Verificar múltiples contaminantes sobre límite
    int contaminantes_sobre_limite = 0;
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        if (zona->historico[DIAS_SIMULACION-1].valores[c] > limites_oms[c]) {
            contaminantes_sobre_limite++;
        }
    }
    
    if (contaminantes_sobre_limite >= 3) {
        return 1; // Acción inmediata requerida
    }
    
    return 0; // No requiere acción inmediata
}

int calcular_tendencia_futura(Zona *zona, int contaminante, Clima clima) {
    if (zona == NULL || contaminante < 0 || contaminante >= NUM_CONTAMINANTES) {
        return 0;
    }
    
    // Obtener tendencia actual
    int tendencia_actual = zona->historico[DIAS_SIMULACION-1].tendencia[contaminante];
    int dias_tendencia = zona->historico[DIAS_SIMULACION-1].dias_consecutivos[contaminante];
    
    // Analizar probabilidad de continuar la tendencia
    float probabilidad_continuar = 0.0;
    
    if (tendencia_actual > 0) { // Subiendo
        probabilidad_continuar = 70.0 - (dias_tendencia * 5.0);
        if (probabilidad_continuar < 30.0) probabilidad_continuar = 30.0;
    } else if (tendencia_actual < 0) { // Bajando
        probabilidad_continuar = 70.0 - (dias_tendencia * 5.0);
        if (probabilidad_continuar < 30.0) probabilidad_continuar = 30.0;
    } else { // Estable
        probabilidad_continuar = 50.0;
    }
    
    // Ajustar por clima
    if (clima.temperatura > 30.0) {
        probabilidad_continuar += 10.0; // Más probabilidad de subir con calor
    }
    
    if (clima.velocidad_viento > 4.0) {
        probabilidad_continuar -= 15.0; // Menos probabilidad de subir con viento
    }
    
    // Determinar tendencia futura basada en probabilidad
    int random_val = rand() % 100;
    
    if (random_val < (int)probabilidad_continuar) {
        return tendencia_actual; // Continuar tendencia actual
    } else {
        // Cambiar tendencia
        if (tendencia_actual > 0) {
            return -1; // De subir a bajar
        } else if (tendencia_actual < 0) {
            return 1; // De bajar a subir
        } else {
            // Si estaba estable, decidir aleatoriamente
            return (rand() % 2 == 0) ? 1 : -1;
        }
    }
}

const char* obtener_nivel_riesgo(int nivel) {
    switch (nivel) {
        case 0: return "MUY BAJO";
        case 1: return "BAJO";
        case 2: return "MODERADO";
        case 3: return "ALTO";
        case 4: return "MUY ALTO";
        case 5: return "EXTREMO";
        default: return "DESCONOCIDO";
    }
}

const char* obtener_color_alerta(int nivel) {
    switch (nivel) {
        case 0: return "\033[32m"; // Verde
        case 1: return "\033[36m"; // Cian
        case 2: return "\033[33m"; // Amarillo
        case 3: return "\033[93m"; // Amarillo brillante
        case 4: return "\033[31m"; // Rojo
        case 5: return "\033[91m"; // Rojo brillante
        default: return "\033[0m"; // Reset
    }
}

const char* obtener_icono_alerta(int nivel) {
    switch (nivel) {
        case 0: return "✅";
        case 1: return "🟢";
        case 2: return "🟡";
        case 3: return "🟠";
        case 4: return "🔴";
        case 5: return "💀";
        default: return "❓";
    }
}

void generar_alerta(Prediccion *pred) {
    if (pred == NULL || pred->zona == NULL) {
        return;
    }
    
    // Calcular nivel de alerta basado en valores actuales y predicciones
    int alertas = 0;
    int emergencias = 0;
    float max_porcentaje = 0.0;
    
    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
        float valor_actual = pred->zona->historico[DIAS_SIMULACION-1].valores[c];
        float valor_predicho = pred->prediccion_24h[c];
        float limite = limites_oms[c];
        
        float porcentaje_actual = (valor_actual / limite) * 100.0;
        float porcentaje_predicho = (valor_predicho / limite) * 100.0;
        
        // Actualizar máximo porcentaje
        if (porcentaje_actual > max_porcentaje) max_porcentaje = porcentaje_actual;
        if (porcentaje_predicho > max_porcentaje) max_porcentaje = porcentaje_predicho;
        
        // Contar alertas y emergencias
        if (porcentaje_actual > 200.0 || porcentaje_predicho > 200.0) {
            emergencias++;
        } else if (porcentaje_actual > 150.0 || porcentaje_predicho > 150.0) {
            alertas += 2;
        } else if (porcentaje_actual > 120.0 || porcentaje_predicho > 120.0) {
            alertas++;
        }
    }
    
    // Determinar nivel de alerta
    if (emergencias > 0 || max_porcentaje > 250.0) {
        pred->nivel_alerta = 3; // Emergencia
        strcpy(pred->alerta, "🚨 EMERGENCIA: Niveles extremadamente peligrosos. ");
        strcat(pred->alerta, "Se recomienda evacuación de grupos sensibles y ");
        strcat(pred->alerta, "suspensión total de actividades al aire libre.");
    } else if (alertas >= 3 || max_porcentaje > 180.0) {
        pred->nivel_alerta = 2; // Alerta alta
        strcpy(pred->alerta, "⚠️ ALERTA ALTA: Niveles peligrosos. ");
        strcat(pred->alerta, "Reducir tráfico vehicular y actividades industriales. ");
        strcat(pred->alerta, "Evitar actividades al aire libre prolongadas.");
    } else if (alertas >= 1 || max_porcentaje > 120.0) {
        pred->nivel_alerta = 1; // Alerta
        strcpy(pred->alerta, "⚠️ ALERTA: Niveles elevados. ");
        strcat(pred->alerta, "Grupos sensibles deben evitar exposición al aire libre. ");
        strcat(pred->alerta, "Monitorear condiciones.");
    } else {
        pred->nivel_alerta = 0; // Normal
        strcpy(pred->alerta, "✅ NORMAL: Calidad del aire dentro de límites aceptables. ");
        strcat(pred->alerta, "Mantener prácticas de monitoreo regular.");
    }
    
    // Verificar si requiere acción inmediata
    if (requiere_accion_inmediata(pred->zona, pred->prediccion_24h)) {
        if (pred->nivel_alerta < 3) {
            pred->nivel_alerta = 3; // Elevar a emergencia si requiere acción
            char temp[100];
            strcpy(temp, pred->alerta);
            strcpy(pred->alerta, "🚨 ACCIÓN INMEDIATA REQUERIDA! ");
            strcat(pred->alerta, temp);
        }
    }
    
    // Agregar información específica de la zona
    char buffer[100];
    snprintf(buffer, sizeof(buffer), " [Zona: %s]", pred->zona->nombre);
    strcat(pred->alerta, buffer);
}