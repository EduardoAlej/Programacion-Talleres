#include "contaminacion.h"
#include "archivos.h"
#include "prediccion.h"

#ifdef _WIN32
#include <windows.h>
#endif

// ==================== PROTOTIPOS DE FUNCIONES LOCALES ====================

void mostrar_banner(void);
void mostrar_menu_principal(void);
void mostrar_resumen_sistema(Zona *zonas, int num_zonas);
void ejecutar_simulacion(Zona *zonas, int num_zonas);
void mostrar_estado_zonas(Zona *zonas, int num_zonas);
void generar_predicciones_24h(Zona *zonas, int num_zonas);
void mostrar_menu_configuracion(Zona *zonas, int num_zonas);
void mostrar_menu_reportes(Zona *zonas, int num_zonas);
void mostrar_creditos(void);
void pausar_ejecucion(void);

// ==================== FUNCIÓN PRINCIPAL ====================

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    Zona *zonas = NULL;
    int num_zonas = 0;
    int opcion;
    
    // Inicializar semilla para números aleatorios
    srand((unsigned int)time(NULL));
    
    // Mostrar banner inicial
    mostrar_banner();
    
    // Intentar cargar datos existentes
    printf("\nCargando sistema...\n");
    cargar_datos_binario(&zonas, &num_zonas);
    
    // Si no hay datos, inicializar nuevo sistema
    if (zonas == NULL) {
        printf("Inicializando nuevo sistema de contaminación...\n");
        inicializar_sistema(&zonas, &num_zonas);
        
        // Generar datos históricos iniciales
        printf("Generando datos históricos (30 días)...\n");
        for (int d = 1; d < DIAS_SIMULACION; d++) {
            for (int z = 0; z < num_zonas; z++) {
                Clima clima = generar_clima_aleatorio();
                simular_datos(&zonas[z], d, clima);
            }
            printf(".");
            if (d % 10 == 0) printf("\n");
        }
        printf("\n");
        
        // Guardar datos iniciales
        guardar_datos_binario(zonas, num_zonas);
        
        printf("✓ Sistema inicializado con éxito.\n");
    } else {
        printf("✓ Sistema cargado correctamente.\n");
    }
    
    // Mostrar resumen inicial
    mostrar_resumen_sistema(zonas, num_zonas);
    
    // Bucle principal del programa
    do {
        mostrar_menu_principal();
        opcion = leer_entero("\nSeleccione una opción: ");
        
        switch (opcion) {
            case 1:
                ejecutar_simulacion(zonas, num_zonas);
                break;
                
            case 2:
                mostrar_estado_zonas(zonas, num_zonas);
                break;
                
            case 3:
                generar_predicciones_24h(zonas, num_zonas);
                break;
                
            case 4:
                printf("\n=== PROMEDIOS HISTÓRICOS ===\n");
                for (int z = 0; z < num_zonas; z++) {
                    mostrar_promedios(&zonas[z]);
                }
                pausar_ejecucion();
                break;
                
            case 5:
                printf("\n=== TENDENCIAS DETALLADAS ===\n");
                for (int z = 0; z < num_zonas; z++) {
                    mostrar_tendencias_detalladas(&zonas[z]);
                }
                pausar_ejecucion();
                break;
                
            case 6:
                mostrar_menu_configuracion(zonas, num_zonas);
                break;
                
            case 7:
                mostrar_menu_reportes(zonas, num_zonas);
                break;
                
            case 8:
                printf("\n=== INFORMACIÓN DEL SISTEMA ===\n");
                mostrar_resumen_sistema(zonas, num_zonas);
                pausar_ejecucion();
                break;
                
            case 9:
                mostrar_creditos();
                pausar_ejecucion();
                break;
                
            case 10:
                printf("\nGuardando datos y saliendo del sistema...\n");
                guardar_datos_binario(zonas, num_zonas);
                guardar_configuracion(zonas, num_zonas);
                liberar_memoria(&zonas, num_zonas);
                printf("¡Hasta pronto! 👋\n");
                break;
                
            default:
                printf("\n❌ Opción no válida. Por favor seleccione una opción del 1 al 10.\n");
                pausar_ejecucion();
        }
        
    } while (opcion != 10);
    
    return 0;
}

// ==================== FUNCIONES AUXILIARES ====================

void mostrar_banner(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                              ║\n");
    printf("║    🏙️  SISTEMA INTEGRAL DE GESTIÓN DE CONTAMINACIÓN 🏙️     ║\n");
    printf("║                                                              ║\n");
    printf("║      Monitoreo, Predicción y Alertas de Calidad del Aire     ║\n");
    printf("║                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void mostrar_menu_principal(void) {
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                     MENÚ PRINCIPAL                         │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│  1. ▶️  Ejecutar simulación (nuevo día)                    │\n");
    printf("│  2. 📊 Mostrar estado actual de todas las zonas            │\n");
    printf("│  3. 🔮 Generar predicciones para 24 horas                  │\n");
    printf("│  4. 📈 Mostrar promedios históricos                        │\n");
    printf("│  5. 📉 Mostrar tendencias detalladas                       │\n");
    printf("│  6. ⚙️  Configuración del sistema                          │\n");
    printf("│  7. 📋 Generar reportes y exportar datos                   │\n");
    printf("│  8. ℹ️  Información del sistema                            │\n");
    printf("│  9. 👥 Créditos y documentación                           │\n");
    printf("│ 10. 🚪 Salir del sistema                                   │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

void mostrar_resumen_sistema(Zona *zonas, int num_zonas) {
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                  RESUMEN DEL SISTEMA                       │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│ Zonas monitoreadas: %-2d                                  │\n", num_zonas);
    printf("│ Días de datos históricos: %-2d                            │\n", DIAS_SIMULACION);
    printf("│ Contaminantes monitoreados: 4 (CO2, SO2, NO2, PM2.5)       │\n");
    printf("│ Última actualización: %-35s │\n", zonas[0].historico[DIAS_SIMULACION-1].fecha);
    printf("├─────────────────────────────────────────────────────────────┤\n");
    
    int total_alertas = 0;
    for (int z = 0; z < num_zonas; z++) {
        int alertas_zona = 0;
        for (int c = 0; c < NUM_CONTAMINANTES; c++) {
            if (zonas[z].historico[DIAS_SIMULACION-1].valores[c] > limites_oms[c]) {
                alertas_zona++;
            }
        }
        total_alertas += alertas_zona;
        
        printf("│ %-20s: %2d alerta(s) activa(s)        │\n", 
               zonas[z].nombre, alertas_zona);
    }
    
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│ Alertas totales: %-2d                                       │\n", total_alertas);
    printf("│ Archivo de datos: %-40s │\n", ARCHIVO_PRINCIPAL);
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

void ejecutar_simulacion(Zona *zonas, int num_zonas) {
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                  SIMULACIÓN DE NUEVO DÍA                   │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    // Generar clima para el nuevo día
    Clima clima_nuevo = generar_clima_aleatorio();
    
    printf("\n🌤️  Condiciones climáticas para el nuevo día:\n");
    printf("   • Temperatura: %.1f°C\n", clima_nuevo.temperatura);
    printf("   • Velocidad del viento: %.1f m/s\n", clima_nuevo.velocidad_viento);
    printf("   • Humedad: %.1f%%\n", clima_nuevo.humedad);
    printf("   • Presión atmosférica: %.1f hPa\n", clima_nuevo.presion_atmosferica);
    
    printf("\n🔄 Actualizando datos para %d zonas...\n", num_zonas);
    
    // Desplazar historial (eliminar el día más antiguo)
    for (int z = 0; z < num_zonas; z++) {
        for (int d = 0; d < DIAS_SIMULACION - 1; d++) {
            zonas[z].historico[d] = zonas[z].historico[d + 1];
        }
    }
    
    // Simular nuevo día para cada zona
    for (int z = 0; z < num_zonas; z++) {
        simular_datos(&zonas[z], DIAS_SIMULACION - 1, clima_nuevo);
        printf("   ✓ %s actualizada\n", zonas[z].nombre);
    }
    
    // Guardar datos actualizados
    guardar_datos_binario(zonas, num_zonas);
    
    printf("\n✅ Simulación completada exitosamente.\n");
    printf("📅 Nuevo día simulado: %s\n", zonas[0].historico[DIAS_SIMULACION-1].fecha);
    
    // Mostrar cambios significativos
    printf("\n📊 Cambios significativos detectados:\n");
    for (int z = 0; z < num_zonas; z++) {
        int cambios_fuertes = 0;
        for (int c = 0; c < NUM_CONTAMINANTES; c++) {
            float valor_anterior = zonas[z].historico[DIAS_SIMULACION-2].valores[c];
            float valor_actual = zonas[z].historico[DIAS_SIMULACION-1].valores[c];
            float cambio_absoluto = fabs(valor_actual - valor_anterior);
            
            if (cambio_absoluto > 3.0) {
                cambios_fuertes++;
            }
        }
        
        if (cambios_fuertes > 0) {
            printf("   • %s: %d contaminante(s) con cambios fuertes\n", 
                   zonas[z].nombre, cambios_fuertes);
        }
    }
    
    pausar_ejecucion();
}

void mostrar_estado_zonas(Zona *zonas, int num_zonas) {
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                ESTADO ACTUAL DE LAS ZONAS                  │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    for (int z = 0; z < num_zonas; z++) {
        mostrar_detalle_zona(&zonas[z]);
        
        // Mostrar índice de calidad del aire
        float aqi = calcular_indice_calidad_aire(&zonas[z]);
        printf("\nÍndice de Calidad del Aire (AQI): %.0f - ", aqi);
        
        if (aqi <= 50) {
            printf("BUENA ✅\n");
        } else if (aqi <= 100) {
            printf("MODERADA 🟡\n");
        } else if (aqi <= 150) {
            printf("DAÑINA A GRUPOS SENSIBLES 🟠\n");
        } else if (aqi <= 200) {
            printf("DAÑINA 🔴\n");
        } else if (aqi <= 300) {
            printf("MUY DAÑINA 🔴🔥\n");
        } else {
            printf("PELIGROSA ☠️\n");
        }
        
        // Separador entre zonas (excepto la última)
        if (z < num_zonas - 1) {
            printf("\n───────────────────────────────────────────────────────────────\n");
        }
    }
    
    pausar_ejecucion();
}

void generar_predicciones_24h(Zona *zonas, int num_zonas) {
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│          PREDICCIONES PARA LAS PRÓXIMAS 24 HORAS           │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    // Generar clima para la predicción
    Clima clima_prediccion = generar_clima_aleatorio();
    
    printf("\n🌤️  Pronóstico del tiempo para las próximas 24h:\n");
    printf("   • Temperatura: %.1f°C\n", clima_prediccion.temperatura);
    printf("   • Velocidad del viento: %.1f m/s\n", clima_prediccion.velocidad_viento);
    printf("   • Humedad: %.1f%%\n", clima_prediccion.humedad);
    printf("   • Presión atmosférica: %.1f hPa\n", clima_prediccion.presion_atmosferica);
    
    // Crear array de predicciones
    Prediccion *predicciones = (Prediccion *)malloc(sizeof(Prediccion) * (size_t)num_zonas);
    if (!predicciones) {
        fprintf(stderr, "Error: no se pudo asignar memoria para predicciones.\n");
        return;
    }
    
    printf("\n🔮 Generando predicciones...\n");
    
    for (int z = 0; z < num_zonas; z++) {
        predicciones[z].zona = &zonas[z];
        predicciones[z].clima_actual = clima_prediccion;
        
        // Calcular predicciones para cada contaminante
        for (int c = 0; c < NUM_CONTAMINANTES; c++) {
            predicciones[z].prediccion_24h[c] = calcular_prediccion(&zonas[z], c, clima_prediccion);
        }
        generar_alerta(&predicciones[z]);
        
        printf("   ✓ %s: Predicciones calculadas\n", zonas[z].nombre);
    }
    
    // Mostrar resultados
    printf("\n📋 RESULTADOS DE PREDICCIÓN:\n");
    printf("───────────────────────────────────────────────────────────────\n");
    
    for (int z = 0; z < num_zonas; z++) {
        printf("\n📍 ZONA: %s\n", zonas[z].nombre);
        printf("───────────────────────────────────────────────────────────────\n");
        
        printf("Contaminante |   Actual   | Predicción  |   Límite   | Estado\n");
        printf("─────────────┼────────────┼─────────────┼────────────┼────────\n");
        
        for (int c = 0; c < NUM_CONTAMINANTES; c++) {
            float actual = zonas[z].historico[DIAS_SIMULACION-1].valores[c];
            float prediccion = predicciones[z].prediccion_24h[c];
            float limite = limites_oms[c];
            
            char estado[20];
            if (prediccion > limite * 1.5) {
                strcpy(estado, "🔴 EMERGENCIA");
            } else if (prediccion > limite) {
                strcpy(estado, "🟠 ALERTA");
            } else if (prediccion > limite * 0.8) {
                strcpy(estado, "🟡 PRECAUCIÓN");
            } else {
                strcpy(estado, "🟢 NORMAL");
            }
            
            printf("%-12s | %10.2f | %11.2f | %10.2f | %s\n",
                   nombres_contaminantes[c], actual, prediccion, limite, estado);
        }
        
        printf("\n%s Nivel de alerta: %s\n",
               obtener_icono_alerta(predicciones[z].nivel_alerta),
               predicciones[z].alerta);
        
        // Mostrar recomendaciones específicas
        char recomendaciones[5][200];
        int num_recomendaciones;
        obtener_recomendaciones(&zonas[z], predicciones[z].prediccion_24h, 
                               recomendaciones, &num_recomendaciones);
        
        if (num_recomendaciones > 0) {
            printf("\n💡 Recomendaciones:\n");
            for (int i = 0; i < num_recomendaciones; i++) {
                printf("   • %s\n", recomendaciones[i]);
            }
        }
        
        if (z < num_zonas - 1) {
            printf("\n───────────────────────────────────────────────────────────────\n");
        }
    }
    
    // Preguntar si desea generar reporte
    printf("\n¿Desea generar un reporte de alertas? (s/n): ");
    char respuesta_buf[8];
    leer_cadena("", respuesta_buf, sizeof(respuesta_buf));
    char respuesta = respuesta_buf[0];
    if (respuesta == 's' || respuesta == 'S') {
        generar_reporte_alerta(zonas, num_zonas, predicciones);
        printf("✅ Reporte de alertas generado exitosamente.\n");
    }
    free(predicciones);
    pausar_ejecucion();
}

void mostrar_menu_configuracion(Zona *zonas, int num_zonas) {
    int opcion_config;
    
    do {
        printf("\n");
        printf("┌─────────────────────────────────────────────────────────────┐\n");
        printf("│                    CONFIGURACIÓN DEL SISTEMA               │\n");
        printf("├─────────────────────────────────────────────────────────────┤\n");
        printf("│  1. 📝 Modificar factores de contaminación por zona        │\n");
        printf("│  2. ⚠️  Ajustar límites de alerta                         │\n");
        printf("│  3. 💾 Guardar configuración actual                        │\n");
        printf("│  4. 📂 Cargar configuración guardada                       │\n");
        printf("│  5. 🗑️  Limpiar archivo histórico                         │\n");
        printf("│  6. 📖 Ver archivo histórico                               │\n");
        printf("│  7. ↩️  Volver al menú principal                           │\n");
        printf("└─────────────────────────────────────────────────────────────┘\n");
        
        opcion_config = leer_entero("\nSeleccione una opción de configuración: ");
        
        switch (opcion_config) {
            case 1:
                printf("\n📝 FACTORES DE CONTAMINACIÓN POR ZONA:\n");
                for (int z = 0; z < num_zonas; z++) {
                    printf("%d. %s: %.2f (actual)\n", 
                           z + 1, zonas[z].nombre, zonas[z].factor_contaminacion);
                }
                
                int zona_a_modificar = leer_entero("\nNúmero de zona a modificar (0 para cancelar): ");
                if (zona_a_modificar > 0 && zona_a_modificar <= num_zonas) {
                    float nuevo_factor = leer_float("Nuevo factor de contaminación (1.0-3.0): ");
                    if (nuevo_factor >= 1.0 && nuevo_factor <= 3.0) {
                        zonas[zona_a_modificar - 1].factor_contaminacion = nuevo_factor;
                        printf("✅ Factor actualizado correctamente.\n");
                    } else {
                        printf("❌ Factor fuera de rango permitido.\n");
                    }
                }
                pausar_ejecucion();
                break;
                
            case 2:
                printf("\n⚠️  LÍMITES DE ALERTA (OMS):\n");
                printf("Nota: Los límites OMS son estándares internacionales.\n");
                printf("Modificarlos requiere justificación científica.\n\n");
                
                for (int c = 0; c < NUM_CONTAMINANTES; c++) {
                    printf("%s: %.2f (actual)\n", 
                           nombres_contaminantes[c], limites_oms[c]);
                }
                
                printf("\n¿Está seguro de modificar los límites? (s/n): ");
                char confirmar;
                scanf(" %c", &confirmar);
                limpiar_buffer();
                
                if (confirmar == 's' || confirmar == 'S') {
                    printf("Ingrese nuevos límites OMS:\n");
                    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
                        char mensaje[50];
                        sprintf(mensaje, "  %s: ", nombres_contaminantes[c]);
                        float nuevo_limite = leer_float(mensaje);
                        if (nuevo_limite > 0) {
                            limites_oms[c] = nuevo_limite;
                        }
                    }
                    printf("✅ Límites actualizados correctamente.\n");
                }
                pausar_ejecucion();
                break;
                
            case 3:
                guardar_configuracion(zonas, num_zonas);
                pausar_ejecucion();
                break;
                
            case 4:
                cargar_configuracion(&zonas, &num_zonas);
                pausar_ejecucion();
                break;
                
            case 5:
                limpiar_archivo_historico();
                pausar_ejecucion();
                break;
                
            case 6:
                leer_archivo_historico();
                pausar_ejecucion();
                break;
                
            case 7:
                printf("Volviendo al menú principal...\n");
                break;
                
            default:
                printf("❌ Opción no válida.\n");
                pausar_ejecucion();
        }
        
    } while (opcion_config != 7);
}

void mostrar_menu_reportes(Zona *zonas, int num_zonas) {
    int opcion_reporte;
    do {
        printf("\n");
        printf("┌─────────────────────────────────────────────────────────────┐\n");
        printf("│                    GENERACIÓN DE REPORTES                   │\n");
        printf("├─────────────────────────────────────────────────────────────┤\n");
        printf("│  1. 📄 Generar reporte CSV                                 │\n");
        printf("│  2. ⚠️  Generar reporte de alertas                         │\n");
        printf("│  3. 📊 Exportar datos a JSON                               │\n");
        printf("│  4. 📈 Generar predicciones con reporte                    │\n");
        printf("│  5. 💾 Guardar estado actual del sistema                   │\n");
        printf("│  6. ↩️  Volver al menú principal                           │\n");
        printf("└─────────────────────────────────────────────────────────────┘\n");
        fflush(stdout);
        opcion_reporte = leer_entero("\nSeleccione tipo de reporte: ");
        
        switch (opcion_reporte) {
            case 1: {
                char nombre_archivo[100];
                printf("Ingrese nombre para el archivo CSV (sin extensión): ");
                leer_cadena("", nombre_archivo, sizeof(nombre_archivo));
                
                if (strlen(nombre_archivo) > 0) {
                    guardar_reporte_csv(zonas, num_zonas, nombre_archivo);
                } else {
                    printf("❌ Nombre de archivo inválido.\n");
                }
                pausar_ejecucion();
                break;
            }
                
            case 2: {
                // Prediccion predicciones[num_zonas];
                Prediccion *predicciones = (Prediccion *)malloc(sizeof(Prediccion) * (size_t)num_zonas);
                if (!predicciones) {
                    fprintf(stderr, "Error: no se pudo asignar memoria para predicciones.\n");
                    pausar_ejecucion();
                    break;
                }
                Clima clima = generar_clima_aleatorio();
                for (int z = 0; z < num_zonas; z++) {
                    predicciones[z].zona = &zonas[z];
                    predicciones[z].clima_actual = clima;
                    for (int c = 0; c < NUM_CONTAMINANTES; c++) {
                        predicciones[z].prediccion_24h[c] = calcular_prediccion(&zonas[z], c, clima);
                    }
                    generar_alerta(&predicciones[z]);
                }
                generar_reporte_alerta(zonas, num_zonas, predicciones);
                free(predicciones);
                pausar_ejecucion();
                break;
            }
                
            case 3: {
                char nombre_archivo[100];
                printf("Ingrese nombre para el archivo JSON (sin extensión): ");
                leer_cadena("", nombre_archivo, sizeof(nombre_archivo));
                
                if (strlen(nombre_archivo) > 0) {
                    exportar_a_json(zonas, num_zonas, nombre_archivo);
                } else {
                    printf("❌ Nombre de archivo inválido.\n");
                }
                pausar_ejecucion();
                break;
            }
                
            case 4:
                generar_predicciones_24h(zonas, num_zonas);
                break;
                
            case 5:
                guardar_datos_binario(zonas, num_zonas);
                pausar_ejecucion();
                break;
                
            case 6:
                printf("Volviendo al menú principal...\n");
                break;
                
            default:
                printf("❌ Opción no válida.\n");
                pausar_ejecucion();
        }
        
    } while (opcion_reporte != 6);
}

void mostrar_creditos(void) {
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                      CRÉDITOS Y LICENCIA                   │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│  🏛️  UNIVERSIDAD DE LAS AMÉRICAS                          │\n");
    printf("│  📚 Programación I - ISWZ1102                              │\n");
    printf("│  📅 Versión 3.0 - 202510                                   │\n");
    printf("│                                                            │\n");
    printf("│  👨‍🏫 Profesor: Carlos Andrés Guaita Ayala                 │\n");
    printf("│  🎓 Estudiantes:                                           │\n");
    printf("│     • Eduardo Alejandro                                    │\n");
    printf("│     • Paulo Fuenmayor                                      │\n");
    printf("│     • Marcelo Basante                                      │\n");
    printf("│                                                            │\n");
    printf("│  📋 DESCRIPCIÓN:                                           │\n");
    printf("│  Sistema Integral de Gestión y Predicción de               │\n");
    printf("│  Contaminación del Aire en Zonas Urbanas                   │\n");
    printf("│                                                            │\n");
    printf("│  ⚙️  CARACTERÍSTICAS:                                      │\n");
    printf("│  • Monitoreo en tiempo real                                │\n");
    printf("│  • Predicción 24 horas                                     │\n");
    printf("│  • Sistema de alertas inteligentes                         │\n");
    printf("│  • Reportes en múltiples formatos                          │\n");
    printf("│  • Persistencia de datos                                   │\n");
    printf("│                                                            │\n");
    printf("│  📄 LICENCIA:                                              │\n");
    printf("│  Este software es para fines educativos.                   │\n");
    printf("│  © 2025 Universidad de las Américas.                       │\n");
    printf("│  Todos los derechos reservados.                            │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

void pausar_ejecucion(void) {
    printf("\nPresione Enter para continuar...");
    fflush(stdout);
    char tmp[4];
    fgets(tmp, sizeof(tmp), stdin); // consume el Enter
}