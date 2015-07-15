/*
 * agent.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef RLAGENT_H_
#define RLAGENT_H_

#include "agent.h"

class RLAgent: public Agent {
public:
	RLAgent(const bool test): Agent(test) {

	}

	virtual ~RLAgent() { }

	Action plan(const State & state);
	virtual double & qvalue(const State &, const int &) = 0;
};

#endif /* AGENT_H_ */
