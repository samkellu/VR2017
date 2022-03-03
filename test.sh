#!/bin/bash

# Trigger all your test cases with this script
gcc vr2017.c -o vr2017

for test in tests/*.in
do
    test_name=$(echo $test | cut -d . -f1)
    echo Running Test: $test_name
    # $test_name.out <(./vr2017 $(cat $test))
    diff $test_name.out <(./vr2017 $(cat $test)) && echo "Test $test_name passed." || echo "Test $test_name failed."
done
