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

class Printer{
private:
	ns3::Ptr<ns3::Node> m_n;
public:
	Printer(ns3::Ptr<ns3::Node> n){
		m_n = n;
		Simulator::Schedule(Seconds(0.99), &Printer::printPit, this);
	}

	void printPit(){
		const nfd::Pit& pit = m_n->GetObject<ndn::L3Protocol>()->getForwarder()->getPit();
		NS_LOG_DEBUG("PIT size: " << pit.size());
		Simulator::Schedule(Seconds(0.5), &Printer::printPit, this);
	}
};

int main(int argc, char* argv[]) {
	Config::SetDefault("ns3::PointToPointNetDevice::DataRate",
			StringValue("500Mbps"));
	Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
	Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("50"));

	//Default command line args values
	std::string format("Inet");
	double time = 2;
	string cDataDelay = "1000ms";
	string pDataDelay = "1000ms";
	string errRate = "0";
	string retx = "1000ms";
	string frequency = "0.1";

	CommandLine cmd;
	cmd.AddValue("time", "Time of the simulation.", time);
	cmd.AddValue("cDataDelay", "Delay of of data responses.", cDataDelay);
	cmd.AddValue("pDataDelay", "Delay of of data responses.", pDataDelay);
	cmd.AddValue("retx", "Retransmission timer.", retx);
	cmd.AddValue("errRate", "Error Rate.", errRate);
	cmd.AddValue("frequency", "Frequency.", frequency);
	cmd.Parse(argc, argv);



	//Creating nodes
	NodeContainer nodes;
	PointToPointHelper p2p;

	nodes.Create(4);

	// Connecting nodes using two links
	NetDeviceContainer d0d2 = p2p.Install(nodes.Get(0), nodes.Get(2));
	NetDeviceContainer d1d2 = p2p.Install(nodes.Get(1), nodes.Get(2));
	NetDeviceContainer d2d3 = p2p.Install(nodes.Get(2), nodes.Get(3));



	// Set loss model
	NS_LOG_DEBUG("Setting error rate to " << errRate);
	Ptr<RateErrorModel> rem = CreateObject<RateErrorModel> ();
	rem->SetAttribute("ErrorRate", StringValue(errRate));
	rem->SetAttribute("ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));

	//p2p.SetDeviceAttribute("ReceiveErrorModel", PointerValue (rem));
	//d2d3.Get(1)->SetAttribute ("ReceiveErrorModel", PointerValue (rem));
	d2d3.Get(0)->SetAttribute ("ReceiveErrorModel", PointerValue (rem));


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
	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerTimersApp");
	consumerHelper.SetPrefix(prefix);
	consumerHelper.SetAttribute("Frequency", StringValue(frequency));
	consumerHelper.SetAttribute("StartTime", StringValue("1s"));
	consumerHelper.SetAttribute("AppDelay", StringValue(cDataDelay));
	consumerHelper.SetAttribute("RetxTimer", StringValue(retx));
	consumerHelper.Install(nodes.Get(0));

	// Producer
	ndn::AppHelper producerHelper("ns3::ndn::ProducerApp");
	// Producer will reply to all requests starting with /prefix
	producerHelper.SetPrefix(prefix);
	producerHelper.SetAttribute("Address", StringValue("/node/3"));
	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
	producerHelper.SetAttribute("AppDelay", StringValue(pDataDelay));
	producerHelper.Install(nodes.Get(3)); // last node

	// Add /prefix origins to ndn::GlobalRouter
	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll();
	//for (ns3::Ptr<ns3::Node> node : nodes) {
	ndnGlobalRoutingHelper.AddOrigins(prefix, nodes.Get(3));
	ndnGlobalRoutingHelper.AddOrigins("/node/3", nodes.Get(3));
	//}

	// Calculate and install FIBs
	ndn::GlobalRoutingHelper::CalculateRoutes();

	//const nfd::Pit& pit = nodes.Get(2)->GetObject<ndn::L3Protocol>()->getForwarder()->getPit();
	Printer p(nodes.Get(2));

	//Print pit size every 1s
	//Simulator::Schedule(Seconds(1), &printPit);
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
