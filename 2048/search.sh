
for n in {0..90}
do
	m=$((600 - $n))
	echo running $m
	echo $m | timeout 3600 time ./2048 || echo "failed $m."
done

