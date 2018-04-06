#!/bin/bash

print_help(){
    echo "The scripts requires a log file as input parameter. The log should be an output from ndn.ConsumerThunks"
}

if [[ $# < 1 ]]
then
    print_help
    exit
fi

while read line
do
    echo ${line}
    time=`echo ${line} | cut -d ' ' -f 1`
    echo "time=${time}"
done < $1





 
