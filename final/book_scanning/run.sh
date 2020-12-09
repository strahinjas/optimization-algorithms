#!/bin/bash

rm -f ../tests/$1*_solution.txt

test_file="$(find ../tests -name "$1*" -type f)"

./book_scanning.exe $test_file