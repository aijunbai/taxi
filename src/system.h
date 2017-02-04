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
#include "HierarchicalAgent.h"

class Agent;

class System {
public:
	System() { }

	double simulate(Agent & agent, bool verbose);
	double simulateHierarchicalAgent(HierarchicalAgent &agent, bool verbose, const unordered_map<string, int> &params = {});

	TaxiEnv env_;
};

#endif /* SYSTEM_H_ */
