/*
 * qlearning.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef QLEARNING_H_
#define QLEARNING_H_

#include "tdagent.h"

/**
 * off-policy Q-learning method
 */
class QLearningAgent: public TemporalDifferenceAgent {
public:
	QLearningAgent(const bool test): TemporalDifferenceAgent("qlearning", test) {

	}

	virtual ~QLearningAgent() {

	}

	virtual void learn(const State & pre_state, int pre_action, double reward, const State & state, int);
	virtual void terminate(const State & state, int action, double reward);

private:
	int greedy(const State & state);
};


#endif /* QLEARNING_H_ */
