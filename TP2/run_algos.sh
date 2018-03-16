echo #!/bin/sh
# Vérifions que nos algorithmes fonctionnent correctement
ex_folder="testset"
algo="progdyn"

echo "algo,taille,serie,temps,hauteur" > ./"results_${algo}".csv

for ex in $(find $ex_folder -type f); do
    size=$(echo $ex | cut -d_ -f1 | cut -d "b" -f2)
    serie=$(echo $ex | cut -d_ -f2 | cut -d "." -f1)
    t=$(./tp.sh -e ./${ex} -a ${algo} -t -h)
    echo $algo,$size,$serie,$t
done >> resultsThreshold.csv