/*
 * race_condition.c - Módulo 2: Demostración de condición de carrera
 *
 * Compilar: gcc -O0 -o race_condition race_condition.c -lpthread
 * Uso:
 *   ./race_condition sin_sync   -> condición de carrera
 *   ./race_condition con_sync   -> comportamiento correcto
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_HILOS   20
#define ITER        500000

/* Variable compartida sin protección */
static long inseguro = 0;
/* Variable compartida con protección */
static long seguro = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

/* ── Hilo sin protección ───────────────────────────────────────── */
static void *sin_sync(void *arg) {
    (void)arg;
    for (int i = 0; i < ITER; i++) {
        /* leer-modificar-escribir sin atómica → race condition */
        long tmp = inseguro;
        tmp++;
        inseguro = tmp;
    }
    return NULL;
}

/* ── Hilo con mutex ───────────────────────────────────────────── */
static void *con_sync(void *arg) {
    (void)arg;
    for (int i = 0; i < ITER; i++) {
        pthread_mutex_lock(&mtx);
        seguro++;
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int modo_sync = (argc > 1 && strcmp(argv[1], "con_sync") == 0);
    long esperado = (long)NUM_HILOS * ITER;
    pthread_t th[NUM_HILOS];

    if (!modo_sync) {
        printf("=== MODO: SIN sincronización (race condition) ===\n");
        printf("Hilos: %d | Iter/hilo: %d | Esperado: %ld\n\n",
               NUM_HILOS, ITER, esperado);
        for (int i = 0; i < NUM_HILOS; i++)
            pthread_create(&th[i], NULL, sin_sync, NULL);
        for (int i = 0; i < NUM_HILOS; i++)
            pthread_join(th[i], NULL);

        long obt = inseguro;
        long per = esperado - obt;
        double pct = (per < 0 ? 0 : 100.0 * per / esperado);
        printf("Resultado obtenido : %ld\n", obt);
        printf("Resultado esperado : %ld\n", esperado);
        printf("Incrementos perdidos: %ld (%.2f%%)\n", (per < 0 ? 0 : per), pct);
        if (obt != esperado)
            printf("\n[!] RACE CONDITION confirmada — resultado INCORRECTO.\n");
        else
            printf("\n[!] Esta ejecución no mostró pérdida (el SO puede serializar hilos).\n"
                   "    Ejecutar varias veces para observar el fenómeno.\n");
    } else {
        printf("=== MODO: CON sincronización (mutex) ===\n");
        printf("Hilos: %d | Iter/hilo: %d | Esperado: %ld\n\n",
               NUM_HILOS, ITER, esperado);
        for (int i = 0; i < NUM_HILOS; i++)
            pthread_create(&th[i], NULL, con_sync, NULL);
        for (int i = 0; i < NUM_HILOS; i++)
            pthread_join(th[i], NULL);

        long obt = seguro;
        printf("Resultado obtenido : %ld\n", obt);
        printf("Resultado esperado : %ld\n", esperado);
        printf("Diferencia         : %ld\n", esperado - obt);
        printf("\n[OK] Sin condición de carrera — datos CORRECTOS.\n");
    }
    return 0;
}
