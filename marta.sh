#! /usr/bin bash

for umbr1 in `seq 0.00 0.05 1.00`
do
    for umbr2 in `seq 0.00 0.05 1.00`
    do
        logrado=`get_pitch --umbr1 $umbr1 --umbr2 $umbr2`
        echo -ne "$umbr1 | $umbr2 | $logrado" > results.txt
    done
done