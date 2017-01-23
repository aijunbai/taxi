/*
 * system.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <cstdlib>
#include <cmath>

#include "taxi.h"
#include "HierarchicalFSMAgent.h"

class Agent;

class System {
public:
	System() { }

	double simulate(Agent & agent, bool verbose);
	double simulateFSM(HierarchicalFSMAgent & agent, bool verbose, bool useStaticTransition);

	TaxiEnv env_;
};

#endif /* SYSTEM_H_ */
