#!/bin/bash

TIME=100

cd ..



for ERR_RATE in 0 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.1 0.11 0.12 0.13 0.14 0.15 0.16 0.17 0.18 0.19 0.2
do
    echo "Running ERR_RATE=${ERR_RATE}"
    #thunks, manually remove the RTT
    cDelay=4960
    pDelay=5000
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=ndn.ProducerThunks:ndn.ConsumerThunks ./waf --run="thunks -retx=10ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/thunks:${ERR_RATE}:${cDelay}:${pDelay}.log

    #net time
    cDelay=1000
    pDelay=5500
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=ndn.ProducerThunks:ndn.ConsumerThunks ./waf --run="thunks -retx=1000ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/net:${ERR_RATE}:${cDelay}:${pDelay}.log
done

ERR_RATE=0
for delay in 1000 1500 2000 2500 3000 3500 4000 4500 5000 5500 6000 6500 7000 7500 8000 8500 9000 9500 10000
do
    pDelay=$delay
    cDelay=$delay
    let cDelay-=40 
    echo $cDelay
    echo "Running Delay=${delay}"
    
#thunks, manually remove the RTT
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=ndn.ProducerThunks:ndn.ConsumerThunks ./waf --run="thunks -retx=10ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/pthunks:${ERR_RATE}:${cDelay}:${pDelay}.log

    #net time
    cDelay=1000
    pDelay=$delay
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=ndn.ProducerThunks:ndn.ConsumerThunks ./waf --run="thunks -retx=1000ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/pnet:${ERR_RATE}:${cDelay}:${pDelay}.log
done

cd scripts

./analyse.sh
./generate_graphs.sh
