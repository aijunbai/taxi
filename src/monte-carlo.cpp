/*
 * monte-carlo.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#include <set>
#include <iostream>

#include "monte-carlo.h"

double & MonteCarloAgent::qvalue(const State & state, const int & action)
{
	return monte_carlo_(state, action).get<0>();
}

void MonteCarloAgent::learn(const State & state, int action, double reward, const State &, int)
{
	history_.push_back(std::make_pair(boost::tuples::make_tuple(state, action), reward));
}

/**
 * episode terminates, update the whole state-action-pair list
 * @param state
 * @param action
 * @param reward
 */
void MonteCarloAgent::terminate(const State & state, int action, double reward)
{
	history_.push_back(std::make_pair(boost::tuples::make_tuple(state, action), reward));

	//calculate returns
	{
		double rewards = 0.0;
		std::list<std::pair<state_action_pair_t, double> >::reverse_iterator it = history_.rbegin();
		for (; it != history_.rend(); ++it) {
			rewards += it->second;
			it->second = rewards;
		}
	}

	//average returns
	{
		std::set<state_action_pair_t> visited;
		std::list<std::pair<state_action_pair_t, double> >::iterator it = history_.begin();
		for (; it != history_.end(); ++it) {
			if (visited.count(it->first)) continue; //should be first visit!

			boost::tuples::tuple<double, u_int64_t> & tuple = monte_carlo_(it->first);

			double & q = tuple.get<0>();
			u_int64_t & n = tuple.get<1>();

			q = (q * n + it->second) / (n + 1);
			n += 1;

			visited.insert(it->first);
		}
	}

	history_.clear(); //prepare for new episode
}
