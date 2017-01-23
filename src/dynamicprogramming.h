/*
 * valueiter.h
 *
 *  Created on: Oct 11, 2011
 *      Author: baj
 */

#ifndef VALUEITER_H_
#define VALUEITER_H_

#include "olagent.h"
#include "table.h"

#include <vector>

using namespace std;

class DPAgent: public OLAgent {
public:
	DPAgent(const bool test);
	virtual ~DPAgent();

	Action plan(const State & state);

private:

	void ValueIteration();

	double UpdateValue(const State & state);
	double UpdateQValue(const State & state, Action action);

	pair<double, Action> Greedy(const State & state);
	HashMap<State, HashMap<int, double>> qtable_;
};

#endif /* VALUEITER_H_ */
