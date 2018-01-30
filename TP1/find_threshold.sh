echo #!/bin/sh
# Vérifions que nos algorithmes fonctionnent correctement
ex_folder="testset"

echo "algo,temps,size" > ./find_threshold.csv

algo="mergesort_threshold"

for threshold in $(seq 1000 10 1200); do
	for ex in $(seq 1 10); do
	    t=$(./tp.sh -e ./testset/testset_100000_${ex}.txt -a $algo -t -T ${threshold})
	    echo $algo,$t,$threshold
	done
done >> find_threshold.csv