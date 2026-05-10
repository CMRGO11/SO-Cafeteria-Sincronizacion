/*
 * cafeteria.c
 * Módulo 1: Simulación de cafetería universitaria con sincronización correcta.
 * Usa mutex, semáforos y variables de condición (pthreads).
 *
 * Compilar: gcc -o cafeteria cafeteria.c -lpthread
 * Ejecutar: ./cafeteria [num_cajeros] [num_cajas] [num_clientes]
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/* ─── Configuración ─────────────────────────────────────────────── */
#define MAX_CAJEROS   10
#define MAX_COLA      100
#define SLEEP_GEN_MS  300   /* ms entre clientes generados */
#define SLEEP_ATN_MS  500   /* ms que tarda un cajero en atender */

/* ─── Estructuras ────────────────────────────────────────────────── */
typedef struct {
    int  id;
    long llegada_ms;
} Cliente;

typedef struct {
    Cliente datos[MAX_COLA];
    int     frente, final, count;
} Cola;

/* ─── Globales sincronizadas ─────────────────────────────────────── */
static Cola           cola;
static pthread_mutex_t mutex_cola   = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond_no_vacia = PTHREAD_COND_INITIALIZER;
static sem_t           sem_cajas;

/* control de fin */
static volatile int generacion_terminada = 0;
static int total_clientes_a_generar      = 0;
static int clientes_atendidos            = 0;
static pthread_mutex_t mutex_contador    = PTHREAD_MUTEX_INITIALIZER;

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

/* ─── Cola (sin sincronización interna; quien llame debe tener mutex) */
static int cola_vacia(void) { return cola.count == 0; }

static void encolar(Cliente c) {
    cola.datos[cola.final] = c;
    cola.final = (cola.final + 1) % MAX_COLA;
    cola.count++;
}

static Cliente desencolar(void) {
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

        pthread_mutex_lock(&mutex_cola);
        encolar(c);
        printf("[GEN]    Cliente %3d llega  | cola=%d\n", c.id, cola.count);
        pthread_cond_signal(&cond_no_vacia);   /* avisa a un cajero */
        pthread_mutex_unlock(&mutex_cola);
    }

    pthread_mutex_lock(&mutex_cola);
    generacion_terminada = 1;
    pthread_cond_broadcast(&cond_no_vacia);    /* despierta cajeros bloqueados */
    pthread_mutex_unlock(&mutex_cola);

    return NULL;
}

/* ─── Hilo cajero ────────────────────────────────────────────────── */
static void *cajero(void *arg) {
    int id_cajero = *(int *)arg;

    while (1) {
        /* 1. Esperar cliente en la cola */
        pthread_mutex_lock(&mutex_cola);
        while (cola_vacia() && !generacion_terminada) {
            pthread_cond_wait(&cond_no_vacia, &mutex_cola);
        }
        if (cola_vacia() && generacion_terminada) {
            pthread_mutex_unlock(&mutex_cola);
            break;
        }
        Cliente c = desencolar();
        pthread_mutex_unlock(&mutex_cola);

        long espera = ahora_ms() - c.llegada_ms;

        /* 2. Pedir caja de atención (semáforo) */
        sem_wait(&sem_cajas);

        printf("[CAJERO %d] Atiende cliente %3d | espera=%ldms\n",
               id_cajero, c.id, espera);

        /* 3. Simular atención */
        sleep_ms(SLEEP_ATN_MS);

        /* 4. Liberar caja */
        sem_post(&sem_cajas);

        /* 5. Actualizar métricas */
        pthread_mutex_lock(&mutex_contador);
        clientes_atendidos++;
        tiempo_espera_total_ms += espera;
        pthread_mutex_unlock(&mutex_contador);
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

    printf("=== Cafetería UDLAP | cajeros=%d cajas=%d clientes=%d ===\n\n",
           num_cajeros, num_cajas, num_clientes);

    sem_init(&sem_cajas, 0, num_cajas);

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
    if (clientes_atendidos > 0)
        printf("Tiempo espera prom.: %ld ms\n",
               tiempo_espera_total_ms / clientes_atendidos);

    sem_destroy(&sem_cajas);
    return 0;
}
