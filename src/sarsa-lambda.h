/*
 * qlearning.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef SARSA_LAMBDA_H_
#define SARSA_LAMBDA_H_

#include "tdagent.h"

/**
 * on-policy Sarsa(lambda) method
 */
class SarsaLambdaAgent: public TemporalDifferenceAgent {
public:
	static const double lambda = 0.9;

public:
	SarsaLambdaAgent(const bool test): TemporalDifferenceAgent("sarsa-lambda", test) {

	}

	virtual ~SarsaLambdaAgent() {

	}

	virtual void learn(const State & pre_state, int pre_action, double reward, const State & state, int);
	virtual void terminate(const State & state, int action, double reward);

private:
	void backup(const State & state, int action, double reward, double post_value);

private:
	std::map<state_action_pair_t, double> eligibility_;
};


#endif /* QLEARNING_H_ */
