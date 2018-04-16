#!/bin/bash

for file in ./logs/*.log
do
    echo $file
    ./generate_stats.sh $file > ./data/data_`basename ${file}`
done
