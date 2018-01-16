for n in {"1000","5000","10000","50000","100000","500000"}; do
	for i in {1..10}; do
		shuf -i 1-$n > testset_${n}_${i}.txt
	done
done