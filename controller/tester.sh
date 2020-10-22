for i in {0..100}
do
./controller localhost 3000 -log ../test -t 1 longsleep
done
