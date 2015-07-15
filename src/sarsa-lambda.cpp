/*
 * qlearning.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#include <set>

#include "utils.h"
#include "sarsa-lambda.h"

void SarsaLambdaAgent::learn(const State & state, int action, double reward, const State & post_state, int post_action)
{
	backup(state, action, reward, qvalue(post_state, post_action));
}

void SarsaLambdaAgent::backup(const State & state, int action, double reward, double post_value)
{
	const double delta = reward + gamma * post_value - qvalue(state, action);

	eligibility_[boost::tuples::make_tuple(state, action)] = 1.0;

	for (std::map<state_action_pair_t, double>::iterator it = eligibility_.begin(); it != eligibility_.end(); ++it) {
		double & eligibility = it->second;

		if (eligibility > 0.0) {
			const State & state = it->first.get<0>();
			int action = it->first.get<1>();

			qvalue(state, action) += alpha * delta * eligibility;
			eligibility *= gamma * lambda;
		}
	}
}

void SarsaLambdaAgent::terminate(const State & state, int action, double reward)
{
	backup(state, action, reward, 0.0);

	eligibility_.clear();
}
