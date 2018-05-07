#!/bin/bash

scp sparta:/space/michal/timers/ns-3/scripts/graphs/*.dat .

for file in *.gp
do
    gnuplot $file
done

#cp *.pdf ~/work/ucl/nets-research/paper-source/thunks/img
