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
	std::string format("Inet");
	std::string input("src/topology-read/examples/small_topo.txt");
	double time = 5;
	string dataDelay = "0";
	string deadline = "0";

	CommandLine cmd;
	cmd.AddValue("time", "Time of the simulation.", time);
	cmd.AddValue("dataDelay", "Delay of of data responses.", dataDelay);
	cmd.AddValue("deadline", "Delay to be added to PIT entries", deadline);
	cmd.Parse(argc, argv);


	NS_LOG_INFO("Reading file: " << input << " format: " << format);

	TopologyReaderHelper topoHelp;
	topoHelp.SetFileName(input);
	topoHelp.SetFileType(format);
	Ptr<TopologyReader> inFile = topoHelp.GetTopologyReader();


	//Creating nodes
	NodeContainer nodes;
	PointToPointHelper p2p;

	nodes.Create(3);

	// Connecting nodes using two links
	p2p.Install(nodes.Get(0), nodes.Get(1));
	p2p.Install(nodes.Get(1), nodes.Get(2));
	/*p2p.Install(nodes.Get(2), nodes.Get(3));*/


	// Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.InstallAll();

	// Choosing forwarding strategy
	ndn::StrategyChoiceHelper::InstallAll("/",
			"/localhost/nfd/strategy/best-route");

	// Install NDN applications
	std::string prefix = "/exec/";


	//Consumer
	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerTimers");
	consumerHelper.SetPrefix(prefix);
	consumerHelper.SetAttribute("Frequency", StringValue("0.1"));
	consumerHelper.SetAttribute("StartTime", StringValue("1s"));
	consumerHelper.SetAttribute("Deadline", StringValue(deadline));
	consumerHelper.Install(nodes.Get(0));

	// Producer
	ndn::AppHelper producerHelper("ns3::ndn::ProducerTimers");
	// Producer will reply to all requests starting with /prefix
	producerHelper.SetPrefix(prefix);
	producerHelper.SetAttribute("Address", StringValue("/node/3"));
	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
	producerHelper.SetAttribute("DataDelay", StringValue(dataDelay));
	producerHelper.Install(nodes.Get(2)); // last node

	// Add /prefix origins to ndn::GlobalRouter
	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll();
	//for (ns3::Ptr<ns3::Node> node : nodes) {
	ndnGlobalRoutingHelper.AddOrigins(prefix, nodes.Get(2));
	ndnGlobalRoutingHelper.AddOrigins("/node/3", nodes.Get(2));
	//}

	// Calculate and install FIBs
	ndn::GlobalRoutingHelper::CalculateRoutes();

	//Run the simulation
	Simulator::Stop(Seconds(time));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

} // namespace ns3

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
