/*
 * olagent.h
 *
 *  Created on: Oct 10, 2011
 *      Author: baj
 */

#ifndef OLAGENT_H_
#define OLAGENT_H_

#include "agent.h"

class OLAgent: public Agent {
public:
	OLAgent(const bool test): Agent(test) { }
	virtual ~OLAgent() { }

	virtual void learn(const State &, int, double, const State &, int) { }
	virtual void terminate(const State &, int, double) { }
};

#endif /* OLAGENT_H_ */
