echo #!/bin/sh
# Vérifions que nos algorithmes fonctionnent correctement
ex_folder="testset"

echo "algo,taille,temps" > ./results.csv

for algo in {"stdsort","mergesort","insertion_sort"}; do
    for ex in $(find $ex_folder -type f); do
        size=$(echo $ex | cut -d_ -f2)
        t=$(./tp.sh -e ./${ex} -a $algo -t)
        echo $algo,$size,$t
    done
done >> results.csv