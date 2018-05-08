#!/bin/bash

declare -A THUNKS_TIME
declare -A THUNKS_PACKETS
for file in data/data_thunks*
do
    ERR_RATE=$(basename -s ".log" $file | sed -e 's/data_thunks//g' | cut -d ':' -f 2 )
    COM_TIME=$(grep "Average completion time:" $file | cut -d ':' -f 2)
    PACKETS=$(grep "Average interests sent:" $file | cut -d ':' -f 2)
    echo "$file -> $ERR_RATE -> $COM_TIME"
    THUNKS_TIME[$ERR_RATE]=$COM_TIME
    THUNKS_PACKETS[$ERR_RATE]=$PACKETS
done

declare -A NET_TIME
declare -A NET_PACKETS
for file in data/data_net*
do
    ERR_RATE=$(basename -s ".log" $file | sed -e 's/data_net//g' | cut -d ':' -f 2  )
    COM_TIME=$(grep "Average completion time:" $file | cut -d ':' -f 2)
    PACKETS=$(grep "Average interests sent:" $file | cut -d ':' -f 2)
    echo "$file -> $ERR_RATE -> $COM_TIME"
    NET_TIME[$ERR_RATE]=$COM_TIME
    NET_PACKETS[$ERR_RATE]=$PACKETS
done

declare -A APP_TIME
declare -A APP_PACKETS
for file in data/data_net*
do
    ERR_RATE=$(basename -s ".log" $file | sed -e 's/data_app//g' | cut -d ':' -f 2  )
    COM_TIME=$(grep "Average data waiting time:" $file | cut -d ':' -f 2)
    PACKETS=$(grep "Average interests sent:" $file | cut -d ':' -f 2)
    echo "$file -> $ERR_RATE -> $COM_TIME"
    APP_TIME[$ERR_RATE]=$COM_TIME
    APP_PACKETS[$ERR_RATE]=$PACKETS
done

IFS=$'\n' SORTED=( $( printf "%s\n" "${!THUNKS_TIME[@]}" | sort -n ) )

echo -e > graphs/satisfaction_loss.dat
echo -e > graphs/packets_loss.dat
for VAL in "${SORTED[@]}"
do 
    echo  $VAL ${THUNKS_TIME[$VAL]} ${NET_TIME[$VAL]} ${APP_TIME[$VAL]} >> graphs/satisfaction_loss.dat
    echo  $VAL ${THUNKS_PACKETS[$VAL]} ${NET_PACKETS[$VAL]} ${APP_PACKETS[$VAL]} >> graphs/packets_loss.dat
done


declare -A PTHUNKS_TIME
declare -A PTHUNKS_PACKETS
for file in data/data_pthunks*
do
    APP_TIME=$(basename -s ".log" $file | sed -e 's/data_pthunks//g' | cut -d ':' -f 4 )
    COM_TIME=$(grep "Average completion time:" $file | cut -d ':' -f 2)
    PACKETS=$(grep "Average interests sent:" $file | cut -d ':' -f 2)
    echo "$file -> $APP_TIME -> $COM_TIME"
    PTHUNKS_TIME[$APP_TIME]=$COM_TIME
    PTHUNKS_PACKETS[$APP_TIME]=$PACKETS
done

declare -A PNET_TIME
declare -A PNET_PACKETS
for file in data/data_pnet*
do
    APP_TIME=$(basename -s ".log" $file | sed -e 's/data_pnet//g' | cut -d ':' -f 4  )
    COM_TIME=$(grep "Average completion time:" $file | cut -d ':' -f 2)
    PACKETS=$(grep "Average interests sent:" $file | cut -d ':' -f 2)
    echo "$file -> $APP_TIME -> $COM_TIME"
    PNET_TIME[$APP_TIME]=$COM_TIME
    PNET_PACKETS[$APP_TIME]=$PACKETS
done

declare -A PAPP_TIME
declare -A PAPP_PACKETS
for file in data/data_papp*
do
    APP_TIME=$(basename -s ".log" $file | sed -e 's/data_pnet//g' | cut -d ':' -f 4  )
    COM_TIME=$(grep "Average data waiting time:" $file | cut -d ':' -f 2)
    PACKETS=$(grep "Average interests sent:" $file | cut -d ':' -f 2)
    echo "$file -> $APP_TIME -> $COM_TIME"
    PAPP_TIME[$APP_TIME]=$COM_TIME
    PAPP_PACKETS[$APP_TIME]=$PACKETS
done


IFS=$'\n' SORTED=( $( printf "%s\n" "${!PTHUNKS_TIME[@]}" | sort -n ) )
echo -e > graphs/satisfaction_generation.dat
echo -e > graphs/packets_generation.dat
for VAL in "${SORTED[@]}"
do 
    echo  $VAL ${PTHUNKS_TIME[$VAL]} ${PNET_TIME[$VAL]} ${PAPP_TIME[$VAL]} >> graphs/satisfaction_generation.dat
    echo  $VAL ${PTHUNKS_PACKETS[$VAL]} ${PNET_PACKETS[$VAL]} >> graphs/packets_generation.dat
done


# STATE / LOSS
declare -A THUNKS_STATE
for file in data/data_state_loss_thunks*
do
    ERR_RATE=$(basename -s ".log" $file  | cut -d ':' -f 2 )
    STATE=$(grep "Max state:" $file | cut -d ':' -f 2)
    echo "$file -> $ERR_RATE -> $STATE"
    THUNKS_STATE[$ERR_RATE]=$STATE
done

declare -A NET_STATE
for file in data/data_state_loss_net*
do
    ERR_RATE=$(basename -s ".log" $file | cut -d ':' -f 2 )
    STATE=$(grep "Max state:" $file | cut -d ':' -f 2)
    echo "$file -> $ERR_RATE -> $STATE"
    NET_STATE[$ERR_RATE]=$STATE
done

declare -A APP_STATE
for file in data/data_state_loss_app*
do
    ERR_RATE=$(basename -s ".log" $file | cut -d ':' -f 2 )
    STATE=$(grep "Max state:" $file | cut -d ':' -f 2)
    echo "$file -> $ERR_RATE -> $STATE"
    APP_STATE[$ERR_RATE]=$STATE
done

IFS=$'\n' SORTED=( $( printf "%s\n" "${!THUNKS_STATE[@]}" | sort -n ) )

echo -e > graphs/state_loss.dat
for VAL in "${SORTED[@]}"
do 
    echo  $VAL ${THUNKS_STATE[$VAL]} ${NET_STATE[$VAL]} ${APP_STATE[$VAL]} >> graphs/state_loss.dat
done


# STATE / EVOLUTION
THUNK_FILE=./logs/state_generation_thunks:0:4960:5000.log
IFS=$'\n'  TIMES=( $( grep "s 2 " $THUNK_FILE  | cut -d ' ' -f 1 | cut -d 's' -f 1 ) )
IFS=$'\n'  VALS=( $( grep "s 2 " $THUNK_FILE  | cut -d ':' -f 4 ) )
for i in `seq 0 "${#TIMES[@]}"`
do
	echo ${TIMES[$i]} ${VALS[$i]} >> ./graphs/state_evolution_thunks.dat
done

NET_FILE=./logs/state_generation_net:0:1000:5000.log
IFS=$'\n'  TIMES=( $( grep "s 2 " $NET_FILE  | cut -d ' ' -f 1 | cut -d 's' -f 1 ) )
echo Times $TIMES
IFS=$'\n'  VALS=( $( grep "s 2 " $NET_FILE  | cut -d ':' -f 4 ) )
echo Vals $VALS
for i in `seq 0 "${#TIMES[@]}"`
do
	echo ${TIMES[$i]} ${VALS[$i]} >> ./graphs/state_evolution_net.dat
done

APP_FILE=./logs/state_generation_app:0:5000:5000.log
IFS=$'\n'  TIMES=( $( grep "s 2 " $APP_FILE  | cut -d ' ' -f 1 | cut -d 's' -f 1 ) )
echo Times $TIMES
IFS=$'\n'  VALS=( $( grep "s 2 " $APP_FILE  | cut -d ':' -f 4 ) )
echo Vals $VALS
for i in `seq 0 "${#TIMES[@]}"`
do
        echo ${TIMES[$i]} ${VALS[$i]} >> ./graphs/state_evolution_app.dat
done

# STATE / DELAY
declare -A THUNKS_STATE
for file in data/data_state_generation_thunks*
do
    APP_TIME=$(basename -s ".log" $file | cut -d ':' -f 4  )
    STATE=$(grep "Max state:" $file | cut -d ':' -f 2)
    echo "$file -> $APP_TIME -> $STATE"
    THUNKS_STATE[$APP_TIME]=$STATE
done

declare -A NET_STATE
for file in data/data_state_generation_net*
do
    APP_TIME=$(basename -s ".log" $file | cut -d ':' -f 4  )
    STATE=$(grep "Max state:" $file | cut -d ':' -f 2)
    echo "$file -> $APP_TIME -> $STATE"
    NET_STATE[$APP_TIME]=$STATE
done

declare -A APP_STATE
for file in data/data_state_generation_app*
do
    APP_TIME=$(basename -s ".log" $file | cut -d ':' -f 4  )
    STATE=$(grep "Max state:" $file | cut -d ':' -f 2)
    echo "$file -> $APP_TIME -> $STATE"
    APP_STATE[$APP_TIME]=$STATE
done

IFS=$'\n' SORTED=( $( printf "%s\n" "${!THUNKS_STATE[@]}" | sort -n ) )

echo -e > graphs/state_generation.dat
for VAL in "${SORTED[@]}"
do 
    echo  $VAL ${THUNKS_STATE[$VAL]} ${NET_STATE[$VAL]} ${APP_STATE[$VAL]} >> graphs/state_generation.dat
done
