#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/topology-read-module.h"
#include "model/ndn-l3-protocol.hpp"
#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"
#include "model/ndn-net-device-transport.hpp"
#include "model/ndn-global-router.hpp"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE("Simulation");

int main(int argc, char* argv[]) {
	Config::SetDefault("ns3::PointToPointNetDevice::DataRate",
			StringValue("500Mbps"));
	Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
	Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("50"));

	//Default command line args values
	double time = 2;
	string cDataDelay = "1000ms";
	string pDataDelay = "1000ms";
	string errRate = "0";
	string retx = "1000ms";
	string frequency = "0.1";
	string format ("Inet");
	string consumers = "1";
	string producers = "1";
	string input ("src/topology-read/examples/Inet_small_toposample.txt");

	CommandLine cmd;
	cmd.AddValue("time", "Time of the simulation.", time);
	cmd.AddValue("cDataDelay", "Delay of of data responses.", cDataDelay);
	cmd.AddValue("pDataDelay", "Delay of of data responses.", pDataDelay);
	cmd.AddValue("retx", "Retransmission timer.", retx);
	cmd.AddValue("errRate", "Error Rate.", errRate);
	cmd.AddValue("frequency", "Frequency.", frequency);
	cmd.AddValue("format", "Format to use for data input [Orbis|Inet|Rocketfuel].", format);
	cmd.AddValue("input", "Name of the input file.", input);
	cmd.AddValue("consumers", "Number of consumers.", consumers);
	cmd.AddValue("producers", "Number of producers.", producers);
	cmd.Parse(argc, argv);


	NS_LOG_INFO("Reading file: " << input << " format: " << format);
	// ------------------------------------------------------------
	// -- Read topology data.
	// --------------------------------------------

	// Pick a topology reader based in the requested format.
	TopologyReaderHelper topoHelp;
	topoHelp.SetFileName (input);
	topoHelp.SetFileType (format);
	Ptr<TopologyReader> inFile = topoHelp.GetTopologyReader();


	NodeContainer nodes;

	if (inFile != 0){
		nodes = inFile->Read ();
	}

	if (inFile->LinksSize () == 0){
		NS_LOG_ERROR ("Problems reading the topology file. Failing.");
	    return -1;
	}

	nodes.Create(4);

	int totlinks = inFile->LinksSize ();

	NS_LOG_INFO ("creating node containers");
	NodeContainer* nc = new NodeContainer[totlinks];
	TopologyReader::ConstLinksIterator iter;
	int i = 0;
	for( iter = inFile->LinksBegin (); iter != inFile->LinksEnd (); iter++, i++ ){
		nc[i] = NodeContainer (iter->GetFromNode(), iter->GetToNode ());
		NS_LOG_INFO("Creating link from " << iter->GetFromNode()->GetId() << "->" << iter->GetToNode()->GetId());
	}

	NS_LOG_INFO ("creating net device containers");
	NetDeviceContainer* ndc = new NetDeviceContainer[totlinks];
	PointToPointHelper p2p;

	for (int i = 0; i < totlinks; i++){
		ndc[i] = p2p.Install (nc[i]);
	}


	// Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.InstallAll();

	// Choosing forwarding strategy
	/*ndn::StrategyChoiceHelper::InstallAll("/",
			"/localhost/nfd/strategy/best-route");*/
	ndn::StrategyChoiceHelper::InstallAll("/",
				"/localhost/nfd/strategy/load-balance");

	// Install NDN applications
	std::string prefix = "/exec/";


	ns3::Ptr<ns3::RandomVariableStream> m_random = ns3::CreateObject<ns3::UniformRandomVariable>();
	m_random->SetAttribute("Min", ns3::DoubleValue(0.0));
	//Consumer
	for(int i = 0; i < stoi(consumers); i++){
        m_random->SetAttribute("Max", ns3::DoubleValue(nodes.size() - 1));
		int chosen = m_random->GetInteger();
		NFD_LOG_DEBUG("Choosing " << chosen << " as consumer");
		ndn::AppHelper consumerHelper("ns3::ndn::ConsumerTimers");
		consumerHelper.SetPrefix(prefix);
		consumerHelper.SetAttribute("Frequency", StringValue(frequency));
		consumerHelper.SetAttribute("StartTime", StringValue("1s"));
		consumerHelper.SetAttribute("AppDelay", StringValue(cDataDelay));
		consumerHelper.SetAttribute("RetxTimer", StringValue(retx));
		//consumerHelper.SetAttribute("StartSeq", StringValue(std::to_string(i*100)));
		consumerHelper.Install(nodes.Get(chosen));
	}

	// Add /prefix origins to ndn::GlobalRouter
	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll();

	for(int i = 0; i < stoi(producers); i++){
		m_random->SetAttribute("Max", ns3::DoubleValue(nodes.size() - 1));
		int chosen = m_random->GetInteger();
		NFD_LOG_DEBUG("Choosing " << chosen << " as producer");
		ndn::AppHelper producerHelper("ns3::ndn::ProducerThunks");
	// Producer will reply to all requests starting with /prefix
		producerHelper.SetPrefix(prefix);
		std::stringstream ss;
		ss << "/node/" << chosen;
		producerHelper.SetAttribute("Address", StringValue(ss.str()));
		producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
		producerHelper.SetAttribute("AppDelay", StringValue(pDataDelay));
		producerHelper.SetAttribute("Loss", StringValue(errRate));
		producerHelper.Install(nodes.Get(chosen)); // last node
		ndnGlobalRoutingHelper.AddOrigins(prefix, nodes.Get(chosen));
		ndnGlobalRoutingHelper.AddOrigins(ss.str(), nodes.Get(chosen));
	}


	//}

	// Calculate and install FIBs
	ndn::GlobalRoutingHelper::CalculateRoutes();

	//Print pit size every 1s
	//Simulator::Schedule(Seconds(1), &printPit);
	//Run the simulation
	Simulator::Stop(Seconds(time));

	Simulator::Run();
	Simulator::Destroy();

	delete[] ndc;
	delete[] nc;


	return 0;
}

} // namespace ns3

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
