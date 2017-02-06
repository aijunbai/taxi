/*
 * monte-carlo.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef MONTE_CARLO_H_
#define MONTE_CARLO_H_

#include <list>

#include "table.h"
#include "rlagent.h"

/**
 * first-visit on-policy Monte Carlo method
 */
class MonteCarloAgent: public RLAgent {
public:
	MonteCarloAgent(const bool test): RLAgent(test) {
	}

	virtual ~MonteCarloAgent() {
	}

	double & qvalue(const State &, const int &);

	virtual void learn(const State & pre_state, int pre_action, double reward, const State &, int);
	virtual void terminate(const State & state, int action, double reward);

private:
	list<pair<pair<State, int>, double> > history_;
	HashMap<State, HashMap<int, tuple<double, u_int64_t>>> monte_carlo_;
};

#endif /* MONTE_CARLO_H_ */
