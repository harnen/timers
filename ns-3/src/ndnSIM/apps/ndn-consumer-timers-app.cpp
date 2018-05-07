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

#include "ndn-consumer-timers-app.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"

NS_LOG_COMPONENT_DEFINE("ndn.ConsumerTimersApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(ConsumerTimersApp);

TypeId
ConsumerTimersApp::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::ConsumerTimersApp")
      .SetGroupName("Ndn")
      .SetParent<ConsumerThunks>()
      .AddConstructor<ConsumerTimersApp>()

      .AddAttribute("Frequency", "Frequency of interest packets", StringValue("1.0"),
                    MakeDoubleAccessor(&ConsumerTimersApp::m_frequency), MakeDoubleChecker<double>())

      .AddAttribute("Randomize",
                    "Type of send time randomization: none (default), uniform, exponential",
                    StringValue("none"),
                    MakeStringAccessor(&ConsumerTimersApp::SetRandomize, &ConsumerTimersApp::GetRandomize),
                    MakeStringChecker())

      .AddAttribute("MaxSeq", "Maximum sequence number to request",
                    IntegerValue(std::numeric_limits<uint32_t>::max()),
                    MakeIntegerAccessor(&ConsumerTimersApp::m_seqMax), MakeIntegerChecker<uint32_t>())

    ;

  return tid;
}

ConsumerTimersApp::ConsumerTimersApp()
  : m_frequency(1.0)
  , m_firstTime(true)
{
  NS_LOG_FUNCTION_NOARGS();
  m_seqMax = std::numeric_limits<uint32_t>::max();
}

ConsumerTimersApp::~ConsumerTimersApp()
{
}

void
ConsumerTimersApp::ScheduleNextPacket()
{
  // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
  // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";
  NS_LOG_DEBUG("Schedule next packet");
  m_seq++;
  if (m_firstTime) {
	  NS_LOG_DEBUG("Scheduling next packet in 0 seconds seq:" << m_seq);
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &ConsumerThunks::SendPacket, this, m_interestName, m_seq);
    m_firstTime = false;
  }
  //we want to keep sending packet no matter what
  else {
	  NS_LOG_DEBUG("Scheduling next packet in " << (1.0 / m_frequency) << " seconds" << "seq: " << m_seq);
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &ConsumerThunks::SendPacket, this, m_interestName, m_seq);
  }
}

void
ConsumerTimersApp::SetRandomize(const std::string& value)
{
  if (value == "uniform") {
    m_random = CreateObject<UniformRandomVariable>();
    m_random->SetAttribute("Min", DoubleValue(0.0));
    m_random->SetAttribute("Max", DoubleValue(2 * 1.0 / m_frequency));
  }
  else if (value == "exponential") {
    m_random = CreateObject<ExponentialRandomVariable>();
    m_random->SetAttribute("Mean", DoubleValue(1.0 / m_frequency));
    m_random->SetAttribute("Bound", DoubleValue(50 * 1.0 / m_frequency));
  }
  else
    m_random = 0;

  m_randomType = value;
}

std::string
ConsumerTimersApp::GetRandomize() const
{
  return m_randomType;
}

} // namespace ndn
} // namespace ns3
