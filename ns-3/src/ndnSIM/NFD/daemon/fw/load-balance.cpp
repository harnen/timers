/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "load-balance.hpp"
#include "algorithm.hpp"
#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/double.h"


namespace nfd {
namespace fw {

NFD_LOG_INIT("LoadBalanceStrategy");


const Name LoadBalanceStrategy::STRATEGY_NAME("ndn:/localhost/nfd/strategy/load-balance/%FD%01");
NFD_REGISTER_STRATEGY(LoadBalanceStrategy);

LoadBalanceStrategy::LoadBalanceStrategy(Forwarder& forwarder, const Name& name)
  : Strategy(forwarder, name)
{
	m_random = ns3::CreateObject<ns3::UniformRandomVariable>();
	m_random->SetAttribute("Min", ns3::DoubleValue(0.0));

}

void
LoadBalanceStrategy::afterReceiveInterest(const Face& inFace, const Interest& interest,
                                        const shared_ptr<pit::Entry>& pitEntry)
{
  const fib::Entry& fibEntry = this->lookupFib(*pitEntry);
  const fib::NextHopList& nexthops = fibEntry.getNextHops();

  /*char* pointer;
  pointer[100041105050] = '4';*/

  NFD_LOG_DEBUG("Received an Interest, got " << nexthops.size() << " hops.");


  //for (fib::NextHopList::const_iterator it = nexthops.begin(); it != nexthops.end(); ++it) {
  while(true){
	  m_random->SetAttribute("Max", ns3::DoubleValue(nexthops.size() - 1));
	  int chosen = m_random->GetInteger();
	  NS_LOG_DEBUG("Randomed: " << chosen);
	  Face& outFace = nexthops.at(chosen).getFace();

	  if (!wouldViolateScope(inFace, interest, outFace) &&
			  canForwardToLegacy(*pitEntry, outFace)) {
		  NFD_LOG_DEBUG("Forwarding on face " << outFace.getId());
		  this->sendInterest(pitEntry, outFace, interest);
		  break;
	  }else{
		  NFD_LOG_DEBUG("Would violate scope - chosing again");
	  }
  }

  if (!hasPendingOutRecords(*pitEntry)) {
    this->rejectPendingInterest(pitEntry);
  }
}

} // namespace fw
} // namespace nfd

