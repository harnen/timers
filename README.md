# Timers

# Install
 	
~~~~
cd ns3
./waf configure --enable-examples
./waf
~~~~

# Simulations
Simulation files are located in `scratch/` folder.
To run, execute:

~~~~
NS_LOG=ndn.Producer:ndn.Consumer ./waf --run=tasks
~~~~

To recreate result from the RICE paper, run scripts located in `ns3/scripts/`:
~~~~
cd ns3/scripts
./run_tests.sh
~~~~

# Stack Changes

* `Deadline` and `Path` fields added in both Interest and Data packets
* `Repeated` field added to the interest. It indicates if its the first packet (recording the path) or the second (using the recorder path for forwarding)
* `forwarder` injects path to every without `repeated` flag set
* For now only `BestRouteStrategy2` checks if `repeated` is set. If it's the case it uses the recorder path for forwarding. 
