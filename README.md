# Proyecto Final — Sistemas Operativos (P26-LIS2062-1)
## Entrega 2: Implementación y Evidencia Técnica

**Universidad de las Américas Puebla**  
Profesor: Edwin Montes Orozco  
Equipo: Luis Carlos Ayuso Guarneros (180613) · Diego Camargo Padilla (180892) · Miguel Angel Garcia Arrieta (181266)

---

## Descripción

Simulación de la cafetería universitaria UDLAP que demuestra:
- **Concurrencia** con hilos POSIX (pthreads)
- **Condiciones de carrera** y su solución con mutex
- **Sincronización** con semáforos y variables de condición

---

## Estructura del repositorio

```
project/
├── src/
│   ├── cafeteria.c          # Módulo 1: simulación de cafetería sincronizada
│   └── race_condition.c     # Módulo 2: demostración de race condition
├── scripts/
│   ├── build.sh             # Compila ambos módulos
│   └── run_experiments.sh   # Ejecuta experimentos y genera logs/tablas
├── results/
│   ├── logs/                # Salidas de ejecución
│   └── tables/              # Tablas CSV con métricas
└── README.md
```

---

## Dependencias

- Linux (Ubuntu 20.04+ recomendado)
- `gcc` (GCC 9+)
- `libpthread` (estándar en Linux, incluida en glibc)

---

## Compilación

```bash
bash scripts/build.sh
```

Produce los binarios `src/cafeteria` y `src/race_condition`.

---

## Ejecución

### Módulo 1 — Simulación de cafetería

```bash
./src/cafeteria [num_cajeros] [num_cajas] [num_clientes]
```

Valores por defecto: `5 cajeros`, `2 cajas`, `20 clientes`.

Ejemplo:
```bash
./src/cafeteria 5 2 20
```

Salida esperada:
```
=== Cafetería UDLAP | cajeros=5 cajas=2 clientes=20 ===
[GEN]    Cliente   1 llega  | cola=1
[CAJERO 1] Atiende cliente   1 | espera=0ms
...
=== RESUMEN ===
Clientes atendidos : 20 / 20
Tiempo espera prom.: 0 ms
```

### Módulo 2 — Race Condition

```bash
# Sin sincronización (muestra el error)
./src/race_condition sin_sync

# Con mutex (muestra el comportamiento correcto)
./src/race_condition con_sync
```

Salida esperada (sin_sync):
```
Resultado obtenido : 7,615,719
Resultado esperado : 10,000,000
Incrementos perdidos: 2,384,281 (23.84%)
[!] RACE CONDITION confirmada — resultado INCORRECTO.
```

Salida esperada (con_sync):
```
Resultado obtenido : 10,000,000
Resultado esperado : 10,000,000
Diferencia         : 0
[OK] Sin condición de carrera — datos CORRECTOS.
```

---

## Ejecutar todos los experimentos

```bash
bash scripts/run_experiments.sh
```

Genera automáticamente:
- `results/logs/` — logs de cada ejecución
- `results/tables/race_condition.csv` — tabla de condición de carrera
- `results/tables/cafeteria_cajeros.csv` — impacto del número de cajeros
- `results/tables/cafeteria_cajas.csv` — impacto del número de cajas

---

## Conceptos de SO demostrados

| Concepto | Dónde |
|---|---|
| Hilos POSIX | `cafeteria.c`, `race_condition.c` |
| Condición de carrera | `race_condition.c` modo `sin_sync` |
| Exclusión mutua (mutex) | `cafeteria.c` — cola de clientes; `race_condition.c` modo `con_sync` |
| Semáforo contable | `cafeteria.c` — cajas de atención |
| Variable de condición | `cafeteria.c` — cajeros en espera activa bloqueada |
| Problema Productor-Consumidor | Generador de clientes ↔ Cajeros |
