# RICE: Remote Method Invocation in ICN
This is a repository containing an implementation of the RICE framework presented in this [paper](https://conferences.sigcomm.org/acm-icn/2018/proceedings/icn18-final9.pdf).

The repository contains an implementation in ndnSim simulator and a prototype based on ndn-cxx.

## ndnSim
 	
~~~~
cd ns3
./waf configure --enable-examples
./waf
~~~~

### Running simulations
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
## Prototype
The prototype showcases both thunks and handshake message exchange. To run the prototype, NFD needs to be installed on hosting machines. The handshake example attempts to send file `test.bin` from to local consumer's directory to producer. The file must be generated before launching `./consumer_handshake`.

To compile the project and generate a random file to send, run: 
~~~~
cd proto
make
`dd if=/dev/random of=test.bin count=100000 bs=1024 iflag=fullblock`
~~~~

Note that the handshake example will work only using one forwarder for both producer and consumer. Running it on different machines requires modifications in NFD. 
