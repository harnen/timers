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

#include "ndn-producer-thunks.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <memory>

NS_LOG_COMPONENT_DEFINE("ndn.ProducerThunks");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(ProducerThunks);

TypeId ProducerThunks::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::ndn::ProducerThunks").SetGroupName("Ndn").SetParent<App>().AddConstructor<
					ProducerThunks>().AddAttribute("Prefix",
					"Prefix, for which producer has the data", StringValue("/"),
					MakeNameAccessor(&ProducerThunks::m_prefix),
					MakeNameChecker()).AddAttribute("Address",
					"Unique prefix identifying the producer", StringValue("/"),
					MakeNameAccessor(&ProducerThunks::m_address),
					MakeNameChecker()).AddAttribute("Postfix",
					"Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
					StringValue("/"),
					MakeNameAccessor(&ProducerThunks::m_postfix),
					MakeNameChecker()).AddAttribute("PayloadSize",
					"Virtual payload size for Content packets",
					UintegerValue(1024),
					MakeUintegerAccessor(&ProducerThunks::m_virtualPayloadSize),
					MakeUintegerChecker<uint32_t>()).AddAttribute("Freshness",
					"Freshness of data packets, if 0, then unlimited freshness",
					TimeValue(Seconds(0)),
					MakeTimeAccessor(&ProducerThunks::m_freshness),
					MakeTimeChecker())
					.AddAttribute("Signature",
						"Fake signature, 0 valid signature (default), other values application-specific",
						UintegerValue(0),
						MakeUintegerAccessor(&ProducerThunks::m_signature),
						MakeUintegerChecker<uint32_t>())
					.AddAttribute("AppDelay",
						"Data Generation Time",
						UintegerValue(2000),
						MakeUintegerAccessor(&ProducerThunks::m_appDelay),
						MakeUintegerChecker<uint32_t>())
					.AddAttribute("KeyLocator",
						"Name to be used for key locator.  If root, then key locator is not used",
						NameValue(),
						MakeNameAccessor(&ProducerThunks::m_keyLocator),
						MakeNameChecker());
	return tid;
}

ProducerThunks::ProducerThunks()
	{
}

// inherited from Application base class.
void ProducerThunks::StartApplication() {
	NS_LOG_FUNCTION_NOARGS();
	App::StartApplication();

	FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
	FibHelper::AddRoute(GetNode(), m_address, m_face, 0);
}

void ProducerThunks::StopApplication() {
	NS_LOG_FUNCTION_NOARGS();

	App::StopApplication();
}

void ProducerThunks::OnInterest(shared_ptr<const Interest> interest) {
	App::OnInterest(interest); // tracing inside


	NS_LOG_FUNCTION(this << *interest);

	if (!m_active) {
		NS_LOG_DEBUG("Producer is not active");
		return;
	}

	NS_LOG_DEBUG("My address:" << m_address);

	Name addrPrefix("/node/");
	NS_LOG_DEBUG("Name in the interest: " << interest->getName());
	if (addrPrefix.isPrefixOf(interest->getName())) {
		NS_LOG_DEBUG("Sending back generated data");
		SendData(interest, 0);
	} else {
		NS_LOG_DEBUG("Sending back my address:" << m_address);
		SendAddress(interest);
	}
}

void ProducerThunks::SendAddress(shared_ptr<const Interest> interest) {
	Name dataName(interest->getName());
	auto data = make_shared<Data>();
	data->setName(dataName);
	data->setFreshnessPeriod(
			::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));

	//create a new "session" for each client with a different ID
	long sessionID = m_sessions.startSession(m_appDelay);
	std::string addr = m_address.toUri()+"/"+std::to_string(sessionID);
	data->setContent(::ndn::encoding::makeStringBlock(::ndn::tlv::Content,
	addr));

	/*if(m_appDelay < 1000){
		NS_LOG_DEBUG("Data will be ready within 1s - " << m_appDelay);
		Simulator::Schedule(MilliSeconds(m_appDelay+1), &ProducerThunks::SendData, this, interest, sessionID);
	}*/


	//data->setContent(m_address.wireEncode());


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

//	  data->setPath(interest->getPath());

	data->setRepeated(interest->getRepeated());

	NS_LOG_INFO(
			"node(" << GetNode()->GetId() << ") responding with Address: " << *data);

	// to create real wire encoding
	data->wireEncode();

	m_transmittedDatas(data, this, m_face);
	m_appLink->onReceiveData(*data);



}

void ProducerThunks::SendData(shared_ptr<const Interest> interest, long sessionID_) {
	Name dataName(interest->getName());

	/*
	 * Get the last component before the sequence number indicating the sessionID and cut the "/"
	 */
	std::string sessionIDs = interest->getName().getSubName(-2, 1).toUri().erase(0, 1);
			sessionID = stol(sessionIDs);
	/*long sessionID;
	if(sessionID_ != 0){
		sessionID =sessionID_;
	}else{

	}*/

	NS_LOG_DEBUG("Extracted sessionID: " << sessionID);

	if(m_sessions.isDataReady(sessionID)){
		NS_LOG_DEBUG("Data ready for the session");
	}else{
		NS_LOG_DEBUG("Data not ready");
		long delay = m_sessions.getRemainingTime(sessionID);
		if(delay == -1){
			NS_LOG_DEBUG("Session does not exist");
			return;
		}
		if(delay < 1000){
			NS_LOG_DEBUG("Data will be ready within 1s - " << delay);
			/* Without "+1" the code enters an infinite loop */
			Simulator::Schedule(MilliSeconds(delay+1), &ProducerThunks::SendData, this, interest, 0);
		}else{
			NS_LOG_DEBUG("Data will NOT be ready within 1s");
		}
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

	m_sessions.stopSession(sessionID);
}

} // namespace ndn
} // namespace ns3
