#!/bin/bash
./a.out | gnuplot -p -e "plot '<cat'"
