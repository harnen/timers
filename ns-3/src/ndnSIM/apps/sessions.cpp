/*
 * sessions.cpp
 *
 *  Created on: 11 Apr 2018
 *      Author: harnen
 */
#include "sessions.hpp"

NS_LOG_COMPONENT_DEFINE("ndn.Sessions");

namespace ns3 {
namespace ndn {

	Sessions::Sessions(){
		sessions = 0;
	}

	bool Sessions::isDataReady(long sessionID){
		std::set<struct session>::iterator it = findSession(sessionID);
		NS_ASSERT(it != m_dataReady.end());
		return it->ready;
	}

	long Sessions::startSession(long generationTime){
		struct session newSession;
		newSession.id = ++sessions;
		newSession.ready = false;
		m_dataReady.insert(newSession);
		NS_LOG_DEBUG("Created session (ID:" << newSession.id << "). Data will be ready in " << generationTime << "ms.");

		Simulator::Schedule(MilliSeconds(generationTime), &Sessions::dataReady, this, sessions);


		return 0;
	}

	void Sessions::dataReady(long sessionID){
		NS_LOG_DEBUG("Data ready. Session ID: " << sessionID);

		std::set<struct session>::iterator it = findSession(sessionID);
		NS_ASSERT(it != m_dataReady.end());
		it->ready = true;
	}

	bool Sessions::stopSession(long sessionID){


		return false;
	}

	std::set<struct session>::iterator Sessions::findSession(long sessionID){
		struct session looking;
		looking.id = sessionID;

		std::set<struct session>::iterator it;
		it = m_dataReady.find(looking);

		return it;

	}



} //namespace ndn
} //namespace ns3
