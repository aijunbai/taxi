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
	ALG_MaxQOP,
	ALG_DynamicProgramming,
	ALG_AStar,
	ALG_UCT,
  ALG_HierarchicalFSM,
	ALG_MaxQ0,
  ALG_MaxQQ
};

inline std::ostream& operator<<(std::ostream& out, const Algorithm value) {
  static std::unordered_map<int, string> strings;

  if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
    INSERT_ELEMENT(ALG_None);
    INSERT_ELEMENT(ALG_MonteCarlo);
    INSERT_ELEMENT(ALG_Sarsa);
    INSERT_ELEMENT(ALG_QLearning);
    INSERT_ELEMENT(ALG_SarsaLambda);
    INSERT_ELEMENT(ALG_MaxQOP);
    INSERT_ELEMENT(ALG_DynamicProgramming);
    INSERT_ELEMENT(ALG_AStar);
    INSERT_ELEMENT(ALG_UCT);
    INSERT_ELEMENT(ALG_HierarchicalFSM);
    INSERT_ELEMENT(ALG_MaxQ0);
    INSERT_ELEMENT(ALG_MaxQQ);
#undef INSERT_ELEMENT
  }

  assert(strings.count(value));
  return out << strings[value];
}

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
