/*
 * cafeteria_race.c
 * Módulo 2: Simulación de cafetería SIN sincronización.
 * Demuestra condición de carrera (race condition).
 *
 * Compilar: gcc -o cafeteria_race cafeteria_race.c -lpthread
 * Ejecutar: ./cafeteria_race [num_cajeros] [num_cajas] [num_clientes]
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/* ─── Configuración ─────────────────────────────────────────────── */
#define MAX_CAJEROS   10
#define MAX_COLA      100
#define SLEEP_GEN_MS  0   /* ms entre clientes generados (más rápido, antes era 1)*/
#define SLEEP_ATN_MS  0   /* ms que tarda un cajero en atender (más rapido, antes era 1 para demostrar race condition)*/

/* ─── Estructuras ────────────────────────────────────────────────── */
typedef struct {
    int  id;
    long llegada_ms;
} Cliente;

typedef struct {
    Cliente datos[MAX_COLA];
    int     frente, final, count;
} Cola;

/* ─── Globales SIN sincronización (⚠️ RACE CONDITION) ────────────── */
static Cola cola;
static int generacion_terminada = 0;
static int total_clientes_a_generar = 0;
static int clientes_atendidos = 0;
static int cajas_ocupadas = 0;

/* métricas */
static long tiempo_espera_total_ms = 0;

/* ─── Utilidades de tiempo ───────────────────────────────────────── */
static long ahora_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

static void sleep_ms(int ms) {
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
}

/* ─── Cola SIN sincronización (⚠️ race condition) ────────────────── */
static int cola_vacia(void) { return cola.count == 0; }

static void encolar(Cliente c) {
    /* ⚠️ Sin mutex: dos hilos pueden escribir al mismo tiempo */
    cola.datos[cola.final] = c;
    cola.final = (cola.final + 1) % MAX_COLA;
    cola.count++;
}

static Cliente desencolar(void) {
    /* ⚠️ Sin mutex: dos hilos pueden leer el mismo cliente */
    Cliente c = cola.datos[cola.frente];
    cola.frente = (cola.frente + 1) % MAX_COLA;
    cola.count--;
    return c;
}

/* ─── Hilo generador de clientes ────────────────────────────────── */
static void *generador(void *arg) {
    int n = *(int *)arg;
    for (int i = 1; i <= n; i++) {
        sleep_ms(SLEEP_GEN_MS);

        Cliente c = { .id = i, .llegada_ms = ahora_ms() };

        /* ⚠️ Sin mutex: posible corrupción de la cola */
        encolar(c);
        printf("[GEN]    Cliente %3d llega  | cola=%d\n", c.id, cola.count);
    }

    generacion_terminada = 1;
    return NULL;
}

/* ─── Hilo cajero SIN sincronización (⚠️ race condition) ─────────── */
static void *cajero(void *arg) {
    int id_cajero = *(int *)arg;

    while (1) {
        /* ⚠️ Sin mutex: puede haber condiciones de carrera aquí */
        if (cola_vacia()) {
            if (generacion_terminada) break;
            sleep_ms(10);  /* espera activa (busy wait) */
            continue;
        }

        /* ⚠️ Sin mutex: dos cajeros pueden tomar el mismo cliente */
        Cliente c = desencolar();
        
        long espera = ahora_ms() - c.llegada_ms;

        /* ⚠️ Sin semáforo: pueden usar más cajas de las disponibles */
        if (cajas_ocupadas >= 0) {  /* Sin control real */
            cajas_ocupadas++;
        }

        printf("[CAJERO %d] Atiende cliente %3d | espera=%ldms | cajas=%d\n",
               id_cajero, c.id, espera, cajas_ocupadas);

        /* Simular atención */
        sleep_ms(SLEEP_ATN_MS);

        /* ⚠️ Sin semáforo: liberación sin control */
        cajas_ocupadas--;
        
        /* ⚠️ Sin mutex: contador compartido sin protección */
        clientes_atendidos++;
        tiempo_espera_total_ms += espera;
    }
    return NULL;
}

/* ─── Main ───────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    int num_cajeros  = (argc > 1) ? atoi(argv[1]) : 5;
    int num_cajas    = (argc > 2) ? atoi(argv[2]) : 2;
    int num_clientes = (argc > 3) ? atoi(argv[3]) : 20;

    total_clientes_a_generar = num_clientes;

    if (num_cajeros > MAX_CAJEROS) num_cajeros = MAX_CAJEROS;

    printf("=== CAFETERÍA CON RACE CONDITION (SIN sincronización) ===\n");
    printf("⚠️  SIN mutex | SIN semáforos | Puede haber errores\n\n");
    printf("Cajeros: %d | Cajas: %d | Clientes: %d\n\n",
           num_cajeros, num_cajas, num_clientes);

    /* Inicializar cola */
    cola.frente = 0;
    cola.final = 0;
    cola.count = 0;
    cajas_ocupadas = 0;

    /* Crear hilo generador */
    pthread_t hilo_gen;
    pthread_create(&hilo_gen, NULL, generador, &num_clientes);

    /* Crear hilos cajeros */
    pthread_t hilos_cajeros[MAX_CAJEROS];
    int ids[MAX_CAJEROS];
    for (int i = 0; i < num_cajeros; i++) {
        ids[i] = i + 1;
        pthread_create(&hilos_cajeros[i], NULL, cajero, &ids[i]);
    }

    /* Esperar a todos */
    pthread_join(hilo_gen, NULL);
    for (int i = 0; i < num_cajeros; i++)
        pthread_join(hilos_cajeros[i], NULL);

    /* Resumen */
    printf("\n=== RESUMEN ===\n");
    printf("Clientes atendidos : %d / %d\n", clientes_atendidos, num_clientes);
    
    if (clientes_atendidos != num_clientes) {
        printf("⚠️  RACE CONDITION DETECTADA: Se perdieron %d clientes\n",
               num_clientes - clientes_atendidos);
    }
    
    if (clientes_atendidos > 0)
        printf("Tiempo espera prom.: %ld ms\n",
               tiempo_espera_total_ms / clientes_atendidos);

    return 0;
}
