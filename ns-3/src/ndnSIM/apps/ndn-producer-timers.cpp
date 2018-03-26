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

#include "ndn-producer-timers.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <memory>

NS_LOG_COMPONENT_DEFINE("ndn.ProducerTimers");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(ProducerTimers);

TypeId ProducerTimers::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::ndn::ProducerTimers").SetGroupName("Ndn").SetParent<App>().AddConstructor<
					ProducerTimers>().AddAttribute("Prefix",
					"Prefix, for which producer has the data", StringValue("/"),
					MakeNameAccessor(&ProducerTimers::m_prefix),
					MakeNameChecker()).AddAttribute("Address",
					"Unique prefix identifying the producer", StringValue("/"),
					MakeNameAccessor(&ProducerTimers::m_address),
					MakeNameChecker()).AddAttribute("Postfix",
					"Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
					StringValue("/"),
					MakeNameAccessor(&ProducerTimers::m_postfix),
					MakeNameChecker()).AddAttribute("PayloadSize",
					"Virtual payload size for Content packets",
					UintegerValue(1024),
					MakeUintegerAccessor(&ProducerTimers::m_virtualPayloadSize),
					MakeUintegerChecker<uint32_t>()).AddAttribute("Freshness",
					"Freshness of data packets, if 0, then unlimited freshness",
					TimeValue(Seconds(0)),
					MakeTimeAccessor(&ProducerTimers::m_freshness),
					MakeTimeChecker())
					.AddAttribute("Signature",
						"Fake signature, 0 valid signature (default), other values application-specific",
						UintegerValue(0),
						MakeUintegerAccessor(&ProducerTimers::m_signature),
						MakeUintegerChecker<uint32_t>())
					.AddAttribute("KeyLocator",
						"Name to be used for key locator.  If root, then key locator is not used",
						NameValue(),
						MakeNameAccessor(&ProducerTimers::m_keyLocator),
						MakeNameChecker())
					.AddAttribute("DataDelay",
							"Delay for Data response (ms)",
							UintegerValue(1000),
							MakeUintegerAccessor(&ProducerTimers::m_dataDelay),
							MakeUintegerChecker<uint32_t>());
	return tid;
}

ProducerTimers::ProducerTimers() {
	NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void ProducerTimers::StartApplication() {
	NS_LOG_FUNCTION_NOARGS();
	App::StartApplication();

	FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
	FibHelper::AddRoute(GetNode(), m_address, m_face, 0);
}

void ProducerTimers::StopApplication() {
	NS_LOG_FUNCTION_NOARGS();

	App::StopApplication();
}

void ProducerTimers::OnInterest(shared_ptr<const Interest> interest) {
	App::OnInterest(interest); // tracing inside

	NS_LOG_FUNCTION(this << *interest);
	NS_LOG_DEBUG(interest);

	if (!m_active) {
		NS_LOG_DEBUG("Producer is not active");
		return;
	}

	NS_LOG_DEBUG("My address:" << m_address);

	Name addrPrefix("/node/");
	NS_LOG_DEBUG("Name in the interest: " << interest->getName());
	if (addrPrefix.isPrefixOf(interest->getName())) {
		NS_LOG_DEBUG("Scheduling sending back data with delay " << m_dataDelay << "ms");
		Simulator::Schedule (MilliSeconds (m_dataDelay), &ProducerTimers::SendData, this, interest);
		//SendData(interest);
	} else {
		NS_LOG_DEBUG("Sending back my address:" << m_address);
		SendAddress(interest);
	}

}

void ProducerTimers::SendAddress(shared_ptr<const Interest> interest) {
	Name dataName(interest->getName());
	auto data = make_shared<Data>();
	data->setName(dataName);
	data->setFreshnessPeriod(
			::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));

	data->setContent(::ndn::encoding::makeStringBlock(::ndn::tlv::Content,
	m_address.toUri()));


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
			"node(" << GetNode()->GetId() << ") responding with Data: " << *data);

	// to create real wire encoding
	data->wireEncode();

	m_transmittedDatas(data, this, m_face);
	m_appLink->onReceiveData(*data);

}

void ProducerTimers::SendData(shared_ptr<const Interest> interest) {
	Name dataName(interest->getName());
	// dataName.append(m_postfix);
	// dataName.appendVersion();

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
			"node(" << GetNode()->GetId() << ") responding with Data: " << *data);

	// to create real wire encoding
	data->wireEncode();

	m_transmittedDatas(data, this, m_face);
	m_appLink->onReceiveData(*data);
}

} // namespace ndn
} // namespace ns3
