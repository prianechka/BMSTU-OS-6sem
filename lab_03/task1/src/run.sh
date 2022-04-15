#!/bin/bash
make
echo ">>>>> Loading md1.ko..."
sleep 0.8
insmod md1.ko
lsmod | grep md1
sleep 1
echo ">>>>> System log after module insert:"
#dmesg
dmesg | tail -50 | grep +
echo ">>>>> Remove md1..."
rmmod md1
sleep 0.8
echo ">>>>> System log after module removal:"
#dmesg
dmesg