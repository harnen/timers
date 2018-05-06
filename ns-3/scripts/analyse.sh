#!/bin/bash

for file in ./logs/*.log
do
    name=`basename $file`
    if  [[ $name == state_loss* ]] ;
    then
        echo state $file
    	./generate_state_stats.sh $file > ./data/data_`basename ${file}`
    else
	echo $file
    	./generate_stats.sh $file > ./data/data_`basename ${file}`
    fi
done
