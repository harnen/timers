#!/bin/bash
print_help(){
    echo "The scripts requires a log file as input parameter. The log should be an output from nfd.PIT"
}

if [[ $# < 1 ]]
then
    print_help
    exit
fi

DEBUG=/dev/null
MAX_STATE=`awk  -F':' 'BEGIN{a=0}  /Max size:/ {if ($4>0+a) a=$6} END{print a}' $1`
echo "Max state: $MAX_STATE"

