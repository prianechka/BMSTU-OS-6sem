#!/bin/bash
make
echo ">>>>> Loading md1.ko..."
sleep 0.8
insmod md1.ko
echo ">>>>> Loading md2.ko..."
sleep 0.8
insmod md2.ko
echo ">>>>> Modules 1 and 2 was loaded. Let's look at them: "
lsmod | grep md
sleep 1
echo ">>>>> This is the last 15 lines in the system log: "
dmesg | tail -15 | grep +
read -n 1 -s -r -p "Press any key to continue..."
echo ">>>>> Remove md2..."
rmmod md2
sleep 0.8
echo ">>>>> Loading md3.ko..."
insmod md3.ko
sleep 0.8
echo ">>>>> This is the last 15 lines in the system log: "
dmesg | tail -15 | grep +
read -n 1 -s -r -p "Press any key to continue..."
echo ">>>>> Remove md3..."rmmod md3
rmmod md3
sleep 0.8
echo ">>>>> Remove md1..."
rmmod md1
sleep 0.8
echo ">>>>> System log after modules removal:"
dmesg | tail -5