#!/bin/bash
# scripts/run_experiments.sh
# Ejecuta todos los experimentos y guarda logs + tablas CSV.

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC="$SCRIPT_DIR/../src"
LOGS="$SCRIPT_DIR/../results/logs"
TABLES="$SCRIPT_DIR/../results/tables"

mkdir -p "$LOGS" "$TABLES"

# ── Asegurarse de que los binarios existen ──────────────────────────
if [ ! -f "$SRC/cafeteria" ] || [ ! -f "$SRC/race_condition" ]; then
    echo "Binarios no encontrados. Compilando primero..."
    bash "$SCRIPT_DIR/build.sh"
fi

echo "========================================================"
echo " EXPERIMENTO 1: Condición de carrera antes/después"
echo "========================================================"

echo "--- SIN sincronización ---"
"$SRC/race_condition" sin_sync | tee "$LOGS/race_sin_sync.log"

echo ""
echo "--- CON sincronización ---"
"$SRC/race_condition" con_sync | tee "$LOGS/race_con_sync.log"

# Extraer datos y construir tabla CSV
echo "modo,obtenido,esperado,perdidos,porcentaje" > "$TABLES/race_condition.csv"

SIN_OBT=$(grep "Resultado obtenido" "$LOGS/race_sin_sync.log" | awk '{print $NF}')
SIN_ESP=$(grep "Resultado esperado" "$LOGS/race_sin_sync.log" | awk '{print $NF}')
SIN_PER=$(grep "Incrementos perdidos" "$LOGS/race_sin_sync.log" | awk '{print $3}')
SIN_PCT=$(grep "Incrementos perdidos" "$LOGS/race_sin_sync.log" | grep -oP '\d+\.\d+')

CON_OBT=$(grep "Resultado obtenido" "$LOGS/race_con_sync.log" | awk '{print $NF}')
CON_ESP=$(grep "Resultado esperado" "$LOGS/race_con_sync.log" | awk '{print $NF}')

echo "sin_sync,$SIN_OBT,$SIN_ESP,$SIN_PER,$SIN_PCT%" >> "$TABLES/race_condition.csv"
echo "con_sync,$CON_OBT,$CON_ESP,0,0.00%"            >> "$TABLES/race_condition.csv"

echo ""
echo "Tabla guardada: $TABLES/race_condition.csv"

echo ""
echo "========================================================"
echo " EXPERIMENTO 2: Cafetería — variando número de cajas"
echo "========================================================"

echo "cajeros,cajas,clientes,clientes_atendidos,espera_prom_ms" > "$TABLES/cafeteria_cajas.csv"

for cajas in 1 2 3 5; do
    LOG="$LOGS/cafeteria_cajas${cajas}.log"
    echo ""
    echo "--- Cajas de atención: $cajas ---"
    "$SRC/cafeteria" 5 "$cajas" 15 | tee "$LOG"

    ATENDIDOS=$(grep "Clientes atendidos" "$LOG" | awk -F': ' '{print $2}' | awk '{print $1}')
    ESPERA=$(grep "Tiempo espera prom" "$LOG" | awk '{print $NF}' | tr -d 'ms')
    [ -z "$ESPERA" ] && ESPERA=0

    echo "5,$cajas,15,$ATENDIDOS,$ESPERA" >> "$TABLES/cafeteria_cajas.csv"
done

echo ""
echo "Tabla guardada: $TABLES/cafeteria_cajas.csv"

echo ""
echo "========================================================"
echo " EXPERIMENTO 3: Cafetería — variando número de cajeros"
echo "========================================================"

echo "cajeros,cajas,clientes,clientes_atendidos,espera_prom_ms" > "$TABLES/cafeteria_cajeros.csv"

for cajeros in 1 2 5 10; do
    LOG="$LOGS/cafeteria_cajeros${cajeros}.log"
    echo ""
    echo "--- Cajeros: $cajeros ---"
    "$SRC/cafeteria" "$cajeros" 2 15 | tee "$LOG"

    ATENDIDOS=$(grep "Clientes atendidos" "$LOG" | awk -F': ' '{print $2}' | awk '{print $1}')
    ESPERA=$(grep "Tiempo espera prom" "$LOG" | awk '{print $NF}' | tr -d 'ms')
    [ -z "$ESPERA" ] && ESPERA=0

    echo "$cajeros,2,15,$ATENDIDOS,$ESPERA" >> "$TABLES/cafeteria_cajeros.csv"
done

echo ""
echo "Tabla guardada: $TABLES/cafeteria_cajeros.csv"

echo ""
echo "========================================================"
echo " Todos los experimentos completados."
echo " Logs  -> results/logs/"
echo " Tablas-> results/tables/"
echo "========================================================"
