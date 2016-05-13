
size=0
if [ -e ../data/data.dat ]
then
    size=$(ls -hl ../data/data.dat | awk -F" " '{print $5}')
fi

if [ $size != "1.0G" ]
then
    echo "generating 1 GB input file.."
    g++ ../data/binary_generator.cpp -o binary_generator.o
    ./binary_generator.o 1 ../data/data.dat > tmp
fi

echo -n "Test 1 [C = 1]: "
echo -e "\nTest 1 [C = 1]: " > tmp
/usr/bin/time --format "%E" ./main.o ../data/data.dat ../data/_data.dat 3000000 1 1 1 >> tmp
cmp <(head -c 100000 ../data/data.dat) <(head -c 100000 ../data/_data.dat) >/dev/null 2>&1
res1=$?
cmp <(tail -c 100000 ../data/data.dat) <(tail -c 100000 ../data/_data.dat) >/dev/null 2>&1
res2=$?
cmp <(head -c 10000000 ../data/data.dat | tail -c 100000) <(head -c 10000000 ../data/_data.dat | tail -c 100000) >/dev/null 2>&1
res3=$?

if [ $[$res1+$res2+$res3] -ne 0 ]
then
    echo "Test 1 failed, please run diff on input and output files."
    exit
fi

echo -n "Test 2 [C = 4]: "
echo -e "\nTest 2 [C = 4]: " >> tmp
/usr/bin/time --format "%E" ./main.o ../data/data.dat ../data/_data.dat 3000000 2 4 1 >> tmp
cmp <(head -c 100000 ../data/data.dat) <(head -c 100000 ../data/_data.dat) >/dev/null 2>&1
res1=$?
cmp <(tail -c 100000 ../data/data.dat) <(tail -c 100000 ../data/_data.dat) >/dev/null 2>&1
res2=$?
cmp <(head -c 10000000 ../data/data.dat | tail -c 100000) <(head -c 10000000 ../data/_data.dat | tail -c 100000) >/dev/null 2>&1
res3=$?

if [ $[$res1+$res2+$res3] -ne 0 ]
then
    echo "Test 2 failed, please run diff on input and output files."
    exit
fi

echo -n "Test 3 [C = 8]: "
echo -e "\nTest 3 [C = 8]: " >> tmp
/usr/bin/time --format "%E" ./main.o ../data/data.dat ../data/_data.dat 3000000 2 8 1 >> tmp
cmp <(head -c 100000 ../data/data.dat) <(head -c 100000 ../data/_data.dat) >/dev/null 2>&1
res1=$?
cmp <(tail -c 100000 ../data/data.dat) <(tail -c 100000 ../data/_data.dat) >/dev/null 2>&1
res2=$?
cmp <(head -c 10000000 ../data/data.dat | tail -c 100000) <(head -c 10000000 ../data/_data.dat | tail -c 100000) >/dev/null 2>&1
res3=$?

if [ $[$res1+$res2+$res3] -ne 0 ]
then
    echo "Test 3 failed, please run diff on input and output files."
    exit
fi

echo "Test succeeded."
