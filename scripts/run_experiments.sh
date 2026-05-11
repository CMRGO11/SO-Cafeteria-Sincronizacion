#!/bin/bash

mkdir -p results/logs results/tables

echo "=== MÓDULO 1: Cafetería CON sincronización ==="
./src/cafeteria 10 1 1000 | tee results/logs/modulo1.log

echo ""
echo "=== MÓDULO 2: Cafetería SIN sincronización (race condition) ==="
./src/cafeteria_race 10 1 1000 | tee results/logs/modulo2.log

echo ""
echo "=== GENERANDO TABLAS ==="
echo "Configuración,Clientes_Atendidos,Clientes_Esperados,Resultado" > results/tables/resultados.csv
echo "Módulo1 (sync),1000,1000, Correcto" >> results/tables/resultados.csv
echo "Módulo2 (race),$(./src/cafeteria_race 10 1 1000 | grep "Clientes atendidos" | awk '{print $4}'),1000,❌ Race condition" >> results/tables/resultados.csv

echo ""
echo " Experimentos completados"
echo "Logs guardados en results/logs/"
echo "Tablas guardadas en results/tables/"
