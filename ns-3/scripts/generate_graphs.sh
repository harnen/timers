#!/bin/bash

declare -A THUNKS_TIME
declare -A THUNKS_PACKETS
for file in data/data_thunks*
do
    ERR_RATE=$(basename -s ".log" $file | sed -e 's/data_thunks//g' )
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
    ERR_RATE=$(basename -s ".log" $file | sed -e 's/data_net//g' )
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
    echo  $VAL ${THUNKS_TIME[$VAL]} ${NET_TIME[$VAL]} >> graphs/satisfaction_time.dat
    echo  $VAL ${THUNKS_PACKETS[$VAL]} ${NET_PACKETS[$VAL]} >> graphs/packets.dat
done
