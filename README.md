# Proyecto Final — Sistemas Operativos (P26-LIS2062-1)
## Implementación y Evidencia Técnica

**Universidad de las Américas Puebla**  
Profesor: Edwin Montes Orozco  
Equipo: Luis Carlos Ayuso Guarneros (180613) · Diego Camargo Padilla (180892) · Miguel Angel Garcia Arrieta (181266)

---

## Descripción del Proyecto

Simulación de una cafetería universitaria donde múltiples hilos (cajeros y un generador de clientes) compiten por recursos limitados (cajas de atención). El sistema demuestra conceptos fundamentales de sistemas operativos como:

- **Condiciones de carrera**
- **Exclcusión con Mutex**
- **Semáforos para control de recursos**
- **Variables de condición**
- **Sincronización entre hilos**

---

## Arquitectura del sistema

| Componente            | Cantidad              | Función                                             |
|-----------------------|-----------------------|-----------------------------------------------------|
| Generador de clientes | 1 hilo                | Crea clientes periódicamente y agregarlos a la fila |
| Cajeros               | Variable (default: 5) | Toman clientes de la fila y los atienden            |
| Cajas de atención     | Variable (default: 3) | Recurso limitado controlado por semáforo            |
| Fila de clientes      | 1 estructura          | Recurso compartido protegido por mutex              |

---

## Estructura del repositorio

```
project/
├── src/
│   ├── cafeteria.c          # Módulo 1: simulación de cafetería sincronizada
│   └── race_condition.c     # Módulo 2: demostración de race condition
├── scripts/
│   ├── build.sh             # Compila ambos módulos
│   └── run_experiments.sh   # Ejecuta experimentos automaticos y genera logs/tablas
├── results/
│   ├── logs/                # Salidas de ejecución
│   └── tables/              # Tablas CSV con métricas
└── README.md
```

---

## Dependencias

- Linux (Ubuntu recomendado)
- `GCC` (compilador de C)
- `libpthread` (biblioteca de hilos POSIX)

---

## Compilación

```bash
# Dar permisos a los scripts
chmod +x scripts/build.sh
chmod +x scripts/run_experiments.sh

# Compilar
bash scripts/build.sh
```

---

## Ejecución

### Módulo 1 — Simulación de cafetería (con sincronización)

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

- **Condición de carrera:** Demostrada en `race_condition.c` al acceder a una variable global sin protección
- **Exclusión mutua (mutex):** Protección de la fila de clientes
- **Semáforos:** Control de acceso a las cajas de atención (recurso limitado)
- **Variables de condición:** Cajeros esperan cuando la fila está vacía
- **Planificación de hilos:** El scheduler de Linux decide qué cajero atiende cada cliente

| Concepto | Dónde se demuestra |
|----------|-------------------|
| Hilos POSIX | `cafeteria.c`, `race_condition.c` |
| Condición de carrera | `race_condition.c` modo `sin_sync` |
| Exclusión mutua (mutex) | `cafeteria.c` — cola de clientes; `race_condition.c` modo `con_sync` |
| Semáforo contable | `cafeteria.c` — cajas de atención (recurso limitado) |
| Variable de condición | `cafeteria.c` — cajeros esperan cuando fila vacía |
| Problema Productor-Consumidor | Generador de clientes (productor) ↔ Cajeros (consumidores) |
| Planificación de hilos | El scheduler de Linux decide qué cajero atiende cada cliente |
