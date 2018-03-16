echo #!/bin/sh
# Vérifions que nos algorithmes fonctionnent correctement
ex_folder="testset"

echo "algo,taille,serie,temps,hauteur" > ./resultsThreshold.csv

algo="vorace"

for ex in $(find $ex_folder -type f); do
    for (( i=0; i<10; i++)); do
        size=$(echo $ex | cut -d_ -f1 | cut -d "b" -f2)
        serie=$(echo $ex | cut -d_ -f2 | cut -d "." -f1)
        seed=$(shuf -i 1-10000000000000 -n 1)
        t=$(./tp.sh -e ./${ex} -a ${algo} -t -h -s ${seed})
        echo $algo,$size,$serie,$t
    done
done >> resultsThreshold.csv