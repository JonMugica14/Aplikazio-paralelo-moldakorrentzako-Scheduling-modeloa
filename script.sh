#!/bin/bash

# Compilar el programa
make

# Lista de parámetros de entrada
parametros=(5 10 50 100)

# Ejecutar el programa con cada parámetro y guardar los resultados en diferentes archivos
for param in "${parametros[@]}"
do
    for i in {1..10}
    do
        
        for j in {0..2}
        do
        ./exec "$param" "$j" >> "resultado_${param}_${j}.txt"
        done
    done
done