/*
 * UCTAgent.h
 *
 *  Created on: Nov 27, 2011
 *      Author: baj
 */

#ifndef UCTAGENT_H_
#define UCTAGENT_H_

#include "olagent.h"
#include "table.h"

class UCTAgent: public OLAgent {
public:
	UCTAgent(const bool test);
	virtual ~UCTAgent();

	Action plan(const State & state);

private:
	Action BestAction(const State & state);

	Action UCT(const State& state);
	double MCTS(const State& state, const int depth);
	double Rollout(State state, int depth);
	Action UCB1(const State & state, const double exploration);

public:
    static double Confidence(int t, int n);

private:
	HashMap<State, HashMap<int, double>> qtable_;
	HashMap<State, int> state_counts_;
	HashMap<State, HashMap<int, int>> state_action_counts_;

	static const double gamma;
};

#endif /* UCTAGENT_H_ */
