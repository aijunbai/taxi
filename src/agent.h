/*
 * agent.h
 *
 *  Created on: Oct 10, 2011
 *      Author: baj
 */

#ifndef AGENT_H_
#define AGENT_H_

#include "policy.h"
#include "taxi.h"

class State;

enum Algorithm {
	ALG_None,

	ALG_MonteCarlo,
	ALG_Sarsa,
	ALG_QLearning,
	ALG_SarsaLambda,
	ALG_MaxQOL,
	ALG_DynamicProgramming,
	ALG_AStar,
	ALG_UCT,
  ALG_HierarchicalFSM
};


class Agent {
public:
	Agent(const bool test): test_(test) {

	}

	virtual ~Agent() { }

	//abstract interfaces
	virtual Action plan(const State & state) = 0;
	virtual void learn(const State &, int, double, const State &, int) = 0; //learning from full quintuple
	virtual void terminate(const State &, int, double) = 0; //inform the agent about termination

public:
	bool test() const { return test_; }
	void set_test(const bool test) { test_ = test; }

private:
	bool test_;
};

#endif /* AGENT_H_ */
