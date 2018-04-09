#!/bin/bash
print_help(){
    echo "The scripts requires a log file as input parameter. The log should be an output from ndn.ConsumerThunks"
}

if [[ $# < 1 ]]
then
    print_help
    exit
fi

MAX_SEQ=`awk  -F':' 'BEGIN{a=0}  /seq:/ {if ($4>0+a) a=$4} END{print a}'  $1`
echo MAX_SEQ: $MAX_SEQ


THUNK_DIFF_SUM=0
THUNK_DIFF_COUNTER=0

DATA_DIFF_SUM=0
DATA_DIFF_COUNTER=0

for i in $(seq 0 $MAX_SEQ)
do
    #echo $i
   if [ $((i%2)) -eq 0 ] 
   then
        TIME_THUNK_REQ=$(grep -a "Sending thunk request.*seq:$i$" $1 | cut -f 1 -d ' ' | cut -f 1 -d 's' | head -n 1)
        #echo First thunk request sent: $TIME_THUNK_REQ 

        TIME_THUNK_REP=$(grep -a "Got a routable address.*seq:$i$" $1 | cut -f 1 -d ' ' | cut -f 1 -d 's' | head -n 1)
        #echo Thunk response received:  $TIME_THUNK_REP

        THUNK_DIFF=`echo "scale=3; ${TIME_THUNK_REP} - ${TIME_THUNK_REQ}" | bc`
        #echo Time thunk difference: $THUNK_DIFF
        
        THUNK_DIFF_SUM=`echo "scale=3; ${THUNK_DIFF_SUM} + ${THUNK_DIFF}" | bc`
        let THUNK_DIFF_COUNTER+=1
    else
        TIME_DATA_REQ=$(grep -a "Sending data request to.*seq:$i$" $1 | cut -f 1 -d ' ' | cut -f 1 -d 's' | head -n 1)
        #echo First data request sent: `echo $TIME_DATA_REQ | cut -d ' ' -f 1`

        TIME_DATA_REP=$(grep -a "Got a data chunk.*seq:$i$" $1 | cut -f 1 -d ' ' | cut -f 1 -d 's' | head -n 1)
        #echo Data response received: `echo $TIME_DATA_REP | cut -d ' ' -f 1`
        
        DATA_DIFF=`echo "scale=3; ${TIME_DATA_REP} - ${TIME_DATA_REQ}" | bc`
        #echo Time data difference: $DATA_DIFF
        
        DATA_DIFF_SUM=`echo "scale=3; ${DATA_DIFF_SUM} + ${DATA_DIFF}" | bc`
        let DATA_DIFF_COUNTER+=1
    fi
done

echo "Average thunk waiting time: `echo "scale=4; ${THUNK_DIFF_SUM}/${THUNK_DIFF_COUNTER}" | bc`"
echo "Average data waiting time: `echo "scale=4; ${DATA_DIFF_SUM}/${DATA_DIFF_COUNTER}" | bc`"
