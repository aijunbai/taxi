/*
 * qlearning.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#include "taxi.h"
#include "utils.h"
#include "qlearning.h"

void QLearningAgent::learn(const State & state, int action, double reward, const State & post_state, int)
{
	double & u = qvalue(state, action);
	const double & v = qvalue(post_state, greedy(post_state));

	u += alpha * (reward + gamma * v - u);
}

void QLearningAgent::terminate(const State & state, int action, double reward)
{
	qvalue(state, action) = reward;
}

int QLearningAgent::greedy(const State & state)
{
	std::vector<double> distri(ActionSize);

	for (int action = 0; action < ActionSize; ++action) {
		distri[action] = qvalue(state, action);
	}

	return PolicyFactory::instance().CreatePolicy(PT_Greedy)->get_action(distri);
}
