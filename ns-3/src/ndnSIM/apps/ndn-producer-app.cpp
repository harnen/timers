/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "ndn-producer-app.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <memory>

NS_LOG_COMPONENT_DEFINE("ndn.ProducerApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(ProducerApp);

TypeId ProducerApp::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::ndn::ProducerApp").SetGroupName("Ndn").SetParent<App>().AddConstructor<
					ProducerApp>().AddAttribute("Prefix",
					"Prefix, for which producer has the data", StringValue("/"),
					MakeNameAccessor(&ProducerApp::m_prefix),
					MakeNameChecker()).AddAttribute("Address",
					"Unique prefix identifying the producer", StringValue("/"),
					MakeNameAccessor(&ProducerApp::m_address),
					MakeNameChecker()).AddAttribute("Postfix",
					"Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
					StringValue("/"),
					MakeNameAccessor(&ProducerApp::m_postfix),
					MakeNameChecker()).AddAttribute("PayloadSize",
					"Virtual payload size for Content packets",
					UintegerValue(1024),
					MakeUintegerAccessor(&ProducerApp::m_virtualPayloadSize),
					MakeUintegerChecker<uint32_t>()).AddAttribute("Freshness",
					"Freshness of data packets, if 0, then unlimited freshness",
					TimeValue(Seconds(0)),
					MakeTimeAccessor(&ProducerApp::m_freshness),
					MakeTimeChecker())
					.AddAttribute("Signature",
						"Fake signature, 0 valid signature (default), other values application-specific",
						UintegerValue(0),
						MakeUintegerAccessor(&ProducerApp::m_signature),
						MakeUintegerChecker<uint32_t>())
					.AddAttribute("AppDelay",
						"Data Generation Time",
						UintegerValue(2000),
						MakeUintegerAccessor(&ProducerApp::m_appDelay),
						MakeUintegerChecker<uint32_t>())
					.AddAttribute("Loss",
						"Loss Rate",
						UintegerValue(0),
						MakeUintegerAccessor(&ProducerApp::m_loss),
						MakeUintegerChecker<uint32_t>())
					.AddAttribute("KeyLocator",
						"Name to be used for key locator.  If root, then key locator is not used",
						NameValue(),
						MakeNameAccessor(&ProducerApp::m_keyLocator),
						MakeNameChecker());
	return tid;
}

ProducerApp::ProducerApp()
	{
	m_lossRandom = CreateObject<UniformRandomVariable>();
	m_lossRandom->SetAttribute("Min", DoubleValue(0.0));
	m_lossRandom->SetAttribute("Max", DoubleValue(100));
}

// inherited from Application base class.
void ProducerApp::StartApplication() {
	NS_LOG_FUNCTION_NOARGS();
	App::StartApplication();

	FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
	FibHelper::AddRoute(GetNode(), m_address, m_face, 0);
}

void ProducerApp::StopApplication() {
	NS_LOG_FUNCTION_NOARGS();

	App::StopApplication();
}

void ProducerApp::OnInterest(shared_ptr<const Interest> interest) {

	if(m_lossRandom->GetInteger() < m_loss){
		NS_LOG_DEBUG("Randomly dropping a packet");
		return;
	}

	App::OnInterest(interest); // tracing inside




	//NS_LOG_FUNCTION(this << *interest);

	if (!m_active) {
		NS_LOG_DEBUG("Producer is not active");
		return;
	}

	uint32_t seq = interest->getName().at(-1).toSequenceNumber();

	//create a new "session" for each client with a different ID
	if(!m_sessions.doesExist(seq)){
		long sessionID = m_sessions.startSession(m_appDelay, seq);
		NS_LOG_DEBUG("sessionID " << sessionID << " seq: " << seq );

		NS_ASSERT(sessionID == (long) seq);
	}

	NS_LOG_DEBUG("Sending back generated data");
	SendData(interest, 0);

}


void ProducerApp::SendData(shared_ptr<const Interest> interest, long dontUse) {
	Name dataName(interest->getName());

	/*
	 * Get the last component before the sequence number indicating the sessionID and cut the "/"
	 */
	uint32_t seq = interest->getName().at(-1).toSequenceNumber();
	long sessionID = seq;
	/*long sessionID;
	if(sessionID_ != 0){
		sessionID =sessionID_;
	}else{

	}*/

	//NS_LOG_DEBUG("Extracted sessionID: " << sessionID);

	if(m_sessions.isDataReady(sessionID)){
		NS_LOG_DEBUG("Data ready for session " << sessionID);
	}else{
		NS_LOG_DEBUG("Data not ready for session " << sessionID);
		long delay = m_sessions.getRemainingTime(sessionID);
		if(delay == -1){
			NS_LOG_DEBUG("Session does not exist " << sessionID);
			return;
		}
		NS_LOG_DEBUG("Data will be ready within 1s - " << delay);
		/* Without "+1" the code enters an infinite loop */
		Simulator::Schedule(MilliSeconds(delay+1), &ProducerApp::SendData, this, interest, 0);

		return;
	}
	auto data = make_shared<Data>();
	data->setName(dataName);
	data->setFreshnessPeriod(
			::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));

	auto buffer = make_shared<::ndn::Buffer>(m_virtualPayloadSize);
	data->setContent(buffer);

	Signature signature;
	SignatureInfo signatureInfo(
			static_cast<::ndn::tlv::SignatureTypeValue>(255));

	if (m_keyLocator.size() > 0) {
		signatureInfo.setKeyLocator(m_keyLocator);
	}

	signature.setInfo(signatureInfo);
	signature.setValue(
			::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue,
					m_signature));

	data->setSignature(signature);

	data->setPath(interest->getPath());

	data->setRepeated(interest->getRepeated());

	NS_LOG_INFO(
			"node(" << GetNode()->GetId() << ") responding with Data");

	// to create real wire encoding
	data->wireEncode();

	m_transmittedDatas(data, this, m_face);
	m_appLink->onReceiveData(*data);

	//m_sessions.stopSession(sessionID);
}

} // namespace ndn
} // namespace ns3
