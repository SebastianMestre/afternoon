if [ $# != 1 ]
then
	echo "usage: $0 <file>"
	exit 1
else
	make --quiet brainguck && ./brainguck $1 > bf.s && gcc -z noexecstack main.c bf.s -o bf && ./bf
fi
