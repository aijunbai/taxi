/*
 * policy.cpp
 *
 *  Created on: Oct 17, 2010
 *      Author: baj
 */

#include <cassert>
#include <algorithm>

#include "policy.h"
#include "utils.h"

Action RandomPolicy::get_action(const std::vector<double> &distri)
{
	return Action(rand() % distri.size());
}

Action GreedyPolicy::get_action(const std::vector<double> &distri)
{
	int best = 0;
	double max = distri[0];

	for (uint i = 1; i < distri.size(); ++i) {
		if (distri[i] > max) {
			max = distri[i];
			best = i;
		}
	}

	return Action(best);
}

Action EpsilonGreedyPolicy::get_action(const std::vector<double> &distri)
{
	if (get_prob() < epsilon_) {
		return Action(rand() % distri.size());
	}
	else {
		return GreedyPolicy::get_action(distri);
	}
}
