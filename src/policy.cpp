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

double EpsilonGreedyPolicy::epsilon_ = 0.01;

int RandomPolicy::get_action(const std::vector<double> &distri)
{
	return int(rand() % distri.size());
}

int GreedyPolicy::get_action(const std::vector<double> &distri)
{
	int bestint = -1;
	double bestValue = -1000000.0;
	int numTies = 0;

	for (uint a = 0; a < distri.size(); ++a) {
		double value = distri[a];

		if (value > bestValue) {
			bestValue = value;
			bestint = a;
		} else if (value == bestValue) {
			numTies++;
			if (rand() % (numTies + 1) == 0) {
				bestValue = value;
				bestint = a;
			}
		}
	}

	assert(bestint != -1);
	return bestint;
}

int EpsilonGreedyPolicy::get_action(const std::vector<double> &distri)
{
	if (get_prob() < epsilon_) {
		return int(rand() % distri.size());
	}
	else {
		return GreedyPolicy::get_action(distri);
	}
}
