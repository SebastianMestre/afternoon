
MKFILE=tester.mk
touch $MKFILE
rm $MKFILE
touch $MKFILE

for n in {0..5}
do
	m=$((525 - $n))
	printf ".PHONY: test$n\n" >> $MKFILE
	printf "test$n:\n" >> $MKFILE
	printf "\techo running $m\n" >> $MKFILE
	printf "\techo $m | timeout 2.5h time ./2048 || echo failed $m\n" >> $MKFILE
done

make -s -f tester.mk test0 test1 test2 test3 test4 test5 -j4
