g++ $1.cpp -o test
echo "Running Test Case for $1"
./test $2
rm -f test