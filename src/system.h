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

class Agent;

class System {
public:
	System() {
		env_.reset();
	}

	System(const State & state) {
		env_.set(state);
	}

	double simulate(Agent & agent, bool verbose = true);

private:
	TaxiEnv env_;
};

#endif /* SYSTEM_H_ */
