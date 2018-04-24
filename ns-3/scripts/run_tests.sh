#!/bin/bash

TIME=20000

cd ..

for ERR_RATE in 0 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.1
do
    echo "Running ERR_RATE=${ERR_RATE}"
    #thunks, manually remove the RTT
    NS_LOG=ndn.ProducerThunks:ndn.ConsumerThunks ./waf --run="thunks -retx=10ms -cDataDelay=5460 -pDataDelay=5500 -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/thunks${ERR_RATE}.log
    #net time
    NS_LOG=ndn.ProducerThunks:ndn.ConsumerThunks ./waf --run="thunks -retx=1000ms -cDataDelay=1000 -pDataDelay=5500 -time=${TIME}s -errRate=${ERR_RATE}" &> ./scripts/logs/net${ERR_RATE}.log
done

