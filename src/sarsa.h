/*
 * qlearning.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef SARSA_H_
#define SARSA_H_

#include "tdagent.h"

/**
 * on-policy Sarsa method
 */
class SarsaAgent: public TemporalDifferenceAgent {
public:
	SarsaAgent(const bool test): TemporalDifferenceAgent("sarsa", test) {

	}

	virtual ~SarsaAgent() {

	}

	virtual void learn(const State & pre_state, int pre_action, double reward, const State & state, int);
	virtual void terminate(const State & state, int action, double reward);
};


#endif /* QLEARNING_H_ */
