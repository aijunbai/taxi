/*
 * agent.cpp
 *
 *  Created on: Oct 17, 2010
 *      Author: baj
 */

#include "rlagent.h"
#include "state.h"
#include "taxi.h"

#include <vector>

Action RLAgent::plan(const State & state) {
	std::vector<double> distri(ActionSize);

	for (int action = 0; action < ActionSize; ++action) {
		distri[action] = qvalue(state, action);
	}

	return PolicyFactory::instance().CreatePolicy(test()? PT_Greedy: PT_EpsilonGreedy)->get_action(distri);
}
