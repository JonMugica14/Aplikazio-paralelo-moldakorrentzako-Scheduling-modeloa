#!/bin/bash

#mpicc -o txip_banaketa_p txip_banaketa_p.c prozesatu_txartela_p.c flag_p.c

programa="txip_banaketa_p"

exek_kop=5
Pkop=2 # Hau aldatu erabili nahi diren prozesu kopurua aldatzeko
mi_array=(2 4 8 12 16 20 24 32 40 52 104)
mkdir -p emaitzak





for((i=0; i<12; i++))
do
emaitzak="emaitzak/emaitzak_${mi_array[i]}.txt"
> "$emaitzak"
for ((j=1; j<=$exek_kop; j++))
do
	Pkop=${mi_array[i]}
    exepar $Pkop $programa txartela >> "$emaitzak"
done
done

echo "Exekuzioak amaitu dira, erantzunak prest daude $emaitzak fitxategian"
