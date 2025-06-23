
for n in {0..100}
do
	m=$((530 - $n))
	echo running $m
	echo $m | timeout 3600 time ./2048 || echo "failed $m."
done

