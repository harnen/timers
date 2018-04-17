/*
 * sessions.hpp
 *
 *  Created on: 11 Apr 2018
 *      Author: harnen
 */

#ifndef SRC_NDNSIM_APPS_SESSIONS_HPP_
#define SRC_NDNSIM_APPS_SESSIONS_HPP_

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

namespace ns3 {
namespace ndn {

struct session{
	long id;
	mutable bool ready;
	bool operator <(const session& s) const{
		return (id < s.id);
	}
};

class Sessions{

public:
	Sessions();
	bool isDataReady(long sessionID);
    bool doesExist(long sessionID);
    long getRemainingTime(long sessionID);

	long startSession(long generationTime);
	void startSession(long generationTime, long sessionID);
	bool stopSession(long sessionID);


private:
	void dataReady(long sessionID);
	std::set<struct session>::iterator findSession(long sessionID);

private:
	std::set<struct session> m_dataReady;
	long sessions;

};

} // namespace ndn
} // namespace ns3





#endif /* SRC_NDNSIM_APPS_SESSIONS_HPP_ */
