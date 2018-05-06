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

IFS=$'\n' SORTED=( $( printf "%s\n" "${!THUNKS_TIME[@]}" | sort -n ) )

echo -e > graphs/satisfaction_time.dat
echo -e > graphs/packets.dat
for VAL in "${SORTED[@]}"
do 
    echo  $VAL ${THUNKS_TIME[$VAL]} ${NET_TIME[$VAL]} >> graphs/satisfaction_generation.dat
    echo  $VAL ${THUNKS_PACKETS[$VAL]} ${NET_PACKETS[$VAL]} >> graphs/packets_loss.dat
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


IFS=$'\n' SORTED=( $( printf "%s\n" "${!PTHUNKS_TIME[@]}" | sort -n ) )
echo -e > graphs/satisfaction_generation.dat
echo -e > graphs/packets_generation.dat
for VAL in "${SORTED[@]}"
do 
    echo  $VAL ${PTHUNKS_TIME[$VAL]} ${PNET_TIME[$VAL]} >> graphs/satisfaction_generation.dat
    echo  $VAL ${PTHUNKS_PACKETS[$VAL]} ${PNET_PACKETS[$VAL]} >> graphs/packets_generation.dat
done
