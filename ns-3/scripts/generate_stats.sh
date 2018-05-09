#!/bin/bash
print_help(){
    echo "The scripts requires a log file as input parameter. The log should be an output from ndn.ConsumerThunks"
}

if [[ $# < 1 ]]
then
    print_help
    exit
fi

DEBUG=/dev/stdout
MAX_SEQ=`awk  -F':' 'BEGIN{a=0}  /seq:/ {if ($4>0+a) a=$4} END{print a}'  $1`
echo MAX_SEQ: $MAX_SEQ


THUNK_DIFF_SUM=0
THUNK_DIFF_COUNTER=0

DATA_DIFF_SUM=0
DATA_DIFF_COUNTER=0

COMPLETION_TIME_SUM=0
COMPLETION_TIME_COUNTER=0

for i in $(seq 1 $MAX_SEQ)
do
   echo $i > $DEBUG
   TIME_THUNK_REQ=$(grep -a "Sending thunk request.*seq:$i$" $1 | cut -f 1 -d ' ' | cut -f 1 -d 's' | head -n 1)
   echo First thunk request sent: $TIME_THUNK_REQ > $DEBUG

   TIME_THUNK_REP=$(grep -a "Got a routable address.*seq:$i$" $1 | cut -f 1 -d ' ' | cut -f 1 -d 's' | head -n 1)
   echo Thunk response received:  $TIME_THUNK_REP > $DEBUG

   THUNK_DIFF=`echo "scale=3; ${TIME_THUNK_REP} - ${TIME_THUNK_REQ}" | bc`
   echo Time thunk difference: $THUNK_DIFF >  $DEBUG
        
   THUNK_DIFF_SUM=`echo "scale=3; ${THUNK_DIFF_SUM} + ${THUNK_DIFF}" | bc`
   let THUNK_DIFF_COUNTER+=1
        
   TIME_DATA_REQ=$(grep -a "Sending data request to.*seq:$i$" $1 | cut -f 1 -d ' ' | cut -f 1 -d 's' | head -n 1)
   echo First data request sent: `echo $TIME_DATA_REQ | cut -d ' ' -f 1` > $DEBUG

   TIME_DATA_REP=$(grep -a "Got a data chunk.*seq:$i$" $1 | cut -f 1 -d ' ' | cut -f 1 -d 's' | head -n 1)
   echo Data response received: `echo $TIME_DATA_REP | cut -d ' ' -f 1` > $DEBUG
        
   if [ -z $TIME_DATA_REP ] 
   then
   	echo "No response reived - ignoring" > $DEBUG
        continue
   fi
        
   DATA_DIFF=`echo "scale=3; ${TIME_DATA_REP} - ${TIME_DATA_REQ}" | bc`
   echo Time data difference: $DATA_DIFF > $DEBUG
        
   DATA_DIFF_SUM=`echo "scale=3; ${DATA_DIFF_SUM} + ${DATA_DIFF}" | bc`
   let DATA_DIFF_COUNTER+=1

   COMPLETION_TIME=`echo "scale=3; ${TIME_DATA_REP} - ${TIME_THUNK_REQ}" | bc`
   COMPLETION_TIME_SUM=`echo "scale=3; ${COMPLETION_TIME_SUM} + ${COMPLETION_TIME}" | bc`
   let COMPLETION_TIME_COUNTER+=1
   echo "Completion time for seq($i): $COMPLETION_TIME" > $DEBUG
done

THUNKS_SENT=`grep -ac "Sending thunk request." $1`
REQUESTS_SENT=`grep -ac "Sending data request." $1`
DATA_REP_SENT=`grep -ac "responding with Data" $1`
ADDR_REP_SENT=`grep -ac "responding with Address" $1`
DATA_RECEIVED=`grep -ac "Got a data chunk" $1`
ACK_RECEIVED=`grep -ac "Got an ACK seq:" $1`
echo "Thunks sent: $THUNKS_SENT"
echo "Requests sent: $REQUESTS_SENT"
echo "Data replies sent: $DATA_REP_SENT"
echo "Data received: $DATA_RECEIVED"
echo "Address replies sent: $ADDR_REP_SENT"
echo "ACKs received: $ACK_RECEIVED"
let TOTAL=$THUNKS_SENT+$REQUESTS_SENT
echo "Total sent: $TOTAL"
echo "Average interests sent: `echo "scale=4; ${TOTAL}/${DATA_RECEIVED}" | bc`"
echo "Average thunk waiting time: `echo "scale=4; ${THUNK_DIFF_SUM}/${THUNK_DIFF_COUNTER}" | bc`"
echo "Average data waiting time: `echo "scale=4; ${DATA_DIFF_SUM}/${DATA_DIFF_COUNTER}" | bc`"
echo "Average completion time: `echo "scale=4; ${COMPLETION_TIME_SUM}/${COMPLETION_TIME_COUNTER}" | bc`"

