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

	bool Sessions::doesExist(long sessionID){
		std::set<struct session>::iterator it = findSession(sessionID);
        if (it != m_dataReady.end()){
          return true;
        }
		return false;
    }

	bool Sessions::isDataReady(long sessionID){
		std::set<struct session>::iterator it = findSession(sessionID);
		if(it == m_dataReady.end()) return false;
		return it->ready;
	}



	long Sessions::getRemainingTime(long sessionID){
		std::set<struct session>::iterator it = findSession(sessionID);
		if(it == m_dataReady.end()) return -1;
		return (it->when - Simulator::Now()).GetMilliSeconds();
	}

	long Sessions::startSession(long generationTime){
		struct session newSession;
		newSession.id = ++sessions;
		newSession.ready = false;
		newSession.when = Simulator::Now() + MilliSeconds(generationTime);
		m_dataReady.insert(newSession);
		NS_LOG_DEBUG("Created session (ID:" << newSession.id << "). Data will be ready in " << generationTime << "ms.");

		Simulator::Schedule(MilliSeconds(generationTime), &Sessions::dataReady, this, sessions);


		return newSession.id;
	}


	void Sessions::startSession(long generationTime, long sessionID){
		struct session newSession;
		newSession.id = sessionID;
		newSession.ready = false;
		newSession.when = Simulator::Now() + MilliSeconds(generationTime);
		m_dataReady.insert(newSession);
		NS_LOG_DEBUG("Created session (ID:" << newSession.id << "). Data will be ready in " << generationTime << "ms.");

		Simulator::Schedule(MilliSeconds(generationTime), &Sessions::dataReady, this, sessions);

		sessions = std::max(sessions, sessionID + 1);
	}

	void Sessions::dataReady(long sessionID){
		NS_LOG_DEBUG("Data ready. Session ID: " << sessionID);

		std::set<struct session>::iterator it = findSession(sessionID);
		NS_ASSERT(it != m_dataReady.end());
		it->ready = true;
	}

	bool Sessions::stopSession(long sessionID){
		NS_LOG_DEBUG("Removing session ID: " << sessionID);

		std::set<struct session>::iterator it = findSession(sessionID);
		NS_ASSERT(it != m_dataReady.end());
		m_dataReady.erase(it);

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
