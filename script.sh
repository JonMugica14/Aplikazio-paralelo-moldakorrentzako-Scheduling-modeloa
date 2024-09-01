#!/bin/bash

#mpicc -o txip_banaketa_p txip_banaketa_p.c prozesatu_txartela_p.c flag_p.c


Pkop=0 # Hau aldatu erabili nahi diren prozesu kopurua aldatzeko
mi_array=(5 10 15 20 25 30)
mkdir -p emaitzak





for((i=0; i<6; i++))
do
mkdir -p "emaitzak/emaitzak_${mi_array[i]}"
emaitzak="emaitzak/emaitzak_${mi_array[i]}/out.txt"
> "$emaitzak"
	Pkop=${mi_array[i]}
    ./exec $Pkop
done


echo "Exekuzioak amaitu dira, erantzunak prest daude $emaitzak fitxategian"
