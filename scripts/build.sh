#!/bin/bash
# scripts/build.sh — Compila ambos módulos

set -e
cd "$(dirname "$0")/../src"

echo "[BUILD] Compilando cafeteria.c ..."
gcc -Wall -Wextra -O2 -o cafeteria cafeteria.c -lpthread
echo "[BUILD] cafeteria OK"

echo "[BUILD] Compilando race_condition.c ..."
gcc -Wall -Wextra -O2 -o race_condition race_condition.c -lpthread
echo "[BUILD] race_condition OK"

echo ""
echo "Binarios listos en src/"
