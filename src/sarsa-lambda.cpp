/*
 * qlearning.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#include <set>

#include "utils.h"
#include "sarsa-lambda.h"

double SarsaLambdaAgent::lambda = 0.9;

void SarsaLambdaAgent::learn(const State & state, int action, double reward, const State & post_state, int post_action)
{
	backup(state, action, reward, qvalue(post_state, post_action));
}

void SarsaLambdaAgent::backup(const State & state, int action, double reward, double post_value)
{
	const double delta = reward + gamma * post_value - qvalue(state, action);

	eligibility_[std::make_pair(state, action)] = 1.0;

	for (auto it = eligibility_.begin(); it != eligibility_.end(); ++it) {
		double & eligibility = it->second;

		if (eligibility > 0.0) {
			const State & state = it->first.first;
			int action = it->first.second;

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
