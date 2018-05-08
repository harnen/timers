#!/bin/bash

TIME=100

rm ./logs/*
rm ./data/*
rm ./graphs/*.dat

cd ..

ERR_RATE=0.2
for delay in `seq 1000 1000 10000`
do
    pDelay=$delay
    cDelay=$delay
    let cDelay-=40 
    echo $cDelay
    frequency=10
    echo "Running state measurements Delay=${delay}"
    
#thunks, manually remove the RTT
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=nfd.PIT ./waf --run="thunks -retx=10ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE} -frequency=${frequency}" &> ./scripts/logs/state_generation_thunks:${ERR_RATE}:${cDelay}:${pDelay}.log

    #net time
    cDelay=1000
    pDelay=$delay
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=nfd.PIT ./waf --run="thunks -retx=1000ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE} -frequency=${frequency}" &> ./scripts/logs/state_generation_net:${ERR_RATE}:${cDelay}:${pDelay}.log
    
    #app time
    cDelay=$delay
    pDelay=$delay
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=nfd.PIT ./waf --run="appTime -retx=10ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE} -frequency=${frequency}" &> ./scripts/logs/state_generation_app:${ERR_RATE}:${cDelay}:${pDelay}.log
done



for ERR_RATE in `seq 0 0.1 0.5`
do
    echo "Running state measurements ERR_RATE=${ERR_RATE}"
    cDelay=4960
    pDelay=5000
    frequency=10	
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=nfd.PIT ./waf --run="thunks -retx=10ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE} -frequency=${frequency}" &> ./scripts/logs/state_loss_thunks:${ERR_RATE}:${cDelay}:${pDelay}.log

    cDelay=1000
    pDelay=5000
    frequency=10	
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=nfd.PIT ./waf --run="thunks -retx=1000ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE} -frequency=${frequency}" &> ./scripts/logs/state_loss_net:${ERR_RATE}:${cDelay}:${pDelay}.log
    
#APPTIME
    cDelay=5000
    pDelay=5000
    frequency=10	
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=nfd.PIT ./waf --run="appTime -retx=1000ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE} -frequency=${frequency}" &> ./scripts/logs/state_loss_app:${ERR_RATE}:${cDelay}:${pDelay}.log
done


for ERR_RATE in `seq 0 0.1 0.5`
do
    echo "Running ERR_RATE=${ERR_RATE}"
    #thunks, manually remove the RTT
    cDelay=4960
    pDelay=5000
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=ndn.ProducerThunks:ndn.ConsumerThunks ./waf --run="thunks -retx=10ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/thunks:${ERR_RATE}:${cDelay}:${pDelay}.log


    #net time
    cDelay=1000
    pDelay=5000
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=ndn.ConsumerTimers:ndn.ProducerThunks:ndn.ConsumerThunks ./waf --run="thunks -retx=1000ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/net:${ERR_RATE}:${cDelay}:${pDelay}.log
    
    #app time
    cDelay=5000
    pDelay=5000
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=ndn.ConsumerTimers:ndn.ProducerApp:ndn.ConsumerApp ./waf --run="appTime -retx=1000ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/app:${ERR_RATE}:${cDelay}:${pDelay}.log
done

ERR_RATE=0
for delay in `seq 1000 1000 10000`
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
    
    #app time
    cDelay=$delay
    pDelay=$delay
    LD_LIBRARY_PATH=/usr/local/lib NS_LOG=ndn.ProducerApp:ndn.ConsumerApp ./waf --run="appTime -retx=1000ms -cDataDelay=${cDelay} -pDataDelay=${pDelay} -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/papp:${ERR_RATE}:${cDelay}:${pDelay}.log
done

cd scripts

./analyse.sh
./generate_graphs.sh
