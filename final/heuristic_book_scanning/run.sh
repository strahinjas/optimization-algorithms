#!/bin/bash

test_file="$(find ../tests -name "$1*" -type f | head -n 1)"

./book_scanning.exe $test_file