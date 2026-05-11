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
project_so/
├── src/
│   ├── cafeteria.c          # Módulo 1: simulación de cafetería sincronizada
│   └── cafeteria_race.c     # Módulo 2: demostración de cafetería con race condition
├── scripts/
│   ├── build.sh             # Compila ambos módulos
│   └── run_experiments.sh   # Ejecuta experimentos automaticos y genera logs/tablas
├── results/
│   ├── logs/                # Salidas de ejecución
│   │   ├── modulo1.log
│   │   └── modulo2.log
│   └── tables/              # Tablas CSV
│   │   └── resultados.csv
│   └── screenshots/
│       ├── modulo1.png
│       ├── modulo2.png
│       └── race_error.png
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

Valores por defecto: `10 cajeros`, `1 cajas`, `1000 clientes`.

Ejemplo:
```bash
./src/cafeteria 10 1 1000
```

Salida esperada:
```
=== Cafetería UDLAP | cajeros=10 cajas=1 clientes=1000 ===
[GEN]    Cliente   1 llega  | cola=1
[CAJERO 1] Atiende cliente   1 | espera=0ms
...
=== RESUMEN ===
Clientes atendidos : 1000 / 1000
Tiempo espera prom.: 0 ms
```

### Módulo 2 — Cafetería SIN sincronización (race condition)

```bash
./src/cafeteria_race [num_cajeros] [num_cajas] [num_clientes]
```

Ejemplo (recomendado para ver el error):
```bash
./src/cafeteria_race 10 1 1000
```

Salida esperada:
```
=== CAFETERÍA CON RACE CONDITION (SIN sincronización) ===
  SIN mutex | SIN semáforos | Puede haber errores

=== RESUMEN ===
Clientes atendidos : 1001 / 1000 
  RACE CONDITION DETECTADA: Se perdieron 1 clientes
```

---

## Scripts disponibles

| Script | Función |
|--------|---------|
| `scripts/build.sh` | Compila ambos módulos |
| `scripts/run_experiments.sh` | Ejecuta los experimentos demostrando ambos módulos |

### Compilar
```bash
bash scripts/build.sh
```

---

## Conceptos de SO demostrados

- **Condición de carrera:** Demostrada en `cafeteria_race.c` al acceder a una variable global sin protección
- **Exclusión mutua (mutex):** Protección de la fila de clientes
- **Semáforos:** Control de acceso a las cajas de atención (recurso limitado)
- **Variables de condición:** Cajeros esperan cuando la fila está vacía
- **Planificación de hilos:** El scheduler de Linux decide qué cajero atiende cada cliente

| Concepto | Dónde se demuestra |
|----------|-------------------|
| Hilos POSIX | `cafeteria.c`, `cafeteria_race.c` |
| Condición de carrera | `cafeteria_race.c` — al ejecutar `./src/cafeteria_race 10 1 1000` |
| Exclusión mutua (mutex) | `cafeteria.c` — protección de la cola de clientes |
| Semáforo contable | `cafeteria.c` — control de acceso a las cajas de atención |
| Variable de condición | `cafeteria.c` — cajeros esperan cuando la fila está vacía |
| Problema Productor-Consumidor | Generador de clientes (productor) ↔ Cajeros (consumidores) |
| Planificación de hilos | El scheduler de Linux decide qué cajero atiende cada cliente |
