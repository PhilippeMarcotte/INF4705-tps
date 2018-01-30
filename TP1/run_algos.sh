echo #!/bin/sh
# Vérifions que nos algorithmes fonctionnent correctement
ex_folder="testset"

echo "algo,taille,temps" > ./resultsThreshold.csv

algo="mergesort_threshold"

for ex in $(find $ex_folder -type f); do
    size=$(echo $ex | cut -d_ -f2)
    t=$(./tp.sh -e ./${ex} -a ${algo} -t -T 100)
    echo $algo,$size,$t
done >> resultsThreshold.csv