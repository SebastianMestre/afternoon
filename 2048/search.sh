
for n in {0..100}
do
	m=$((600 - $n))
	echo running $m
	echo $m | timeout 60 time ./2048 || echo "failed $m."
done

