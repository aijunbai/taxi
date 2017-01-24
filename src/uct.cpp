/*
 * UCTAgent.cpp
 *
 *  Created on: Nov 27, 2011
 *      Author: baj
 */

#include "uct.h"

#define DEBUG 0

const double UCTAgent::gamma = 0.95;

UCTAgent::UCTAgent(const bool test): OLAgent(test) {
	// TODO Auto-generated constructor stub

}

UCTAgent::~UCTAgent() {
	// TODO Auto-generated destructor stub
}

Action UCTAgent::plan(const State & state)
{
	return UCT(state);
}

Action UCTAgent::UCT(const State& state)
{
	const int trials = std::pow(2, 10);

	state_counts_[state] = 1;

	for (int i = 0; i < trials; ++i) {
#if DEBUG
		double reward = MCTS(state, 0);
		std::cout << ": " << reward <<std::endl;
		std::cout << std::endl;
#else
		MCTS(state, 0);
#endif
	}

	return BestAction(state);
}

Action UCTAgent::BestAction(const State & state)
{
	return UCB1(state, 0.0);
}

Action UCTAgent::UCB1(const State & state, const double exploration)
{
	double max = -1.0e6;
	std::vector<int> ties;

	for (int i = 0; i < ActionSize; ++i) {
		double tmp = qtable_[state][i] + exploration * Confidence(state_counts_[state], state_action_counts_[state][i]);

		if (tmp >= max) {
			if (tmp > max) {
				max = tmp;
				ties.clear();
			}
			ties.push_back(i);
		}
	}

	return Action(ties[rand() % ties.size()]);
}

double UCTAgent::MCTS(const State& state, const int depth) //FIXME: no rollout phase!
{
	if (state.terminated() || depth > 1.0 / (1.0 - gamma)) {
#if DEBUG
		std::cout << "[" << state << "]";
#endif
		return 0.0;
	}

	if (state_counts_.count(state)) {
	  Action action = UCB1(state, 10.0);
	  std::pair<State, double> sample = TaxiEnv::Sample(state, action);

	#if DEBUG
	  std::cout << "[" << state << ", " << action_name(action) << "] -> ";
	#endif

	  int n = state_action_counts_[state][action];
	  double u = qtable_[state][action];
	  double v = sample.second + gamma * MCTS(sample.first, depth + 1);
	  double k = 1.0 / (n + 1);
	  double w = u + k * (v - u);

	  state_counts_[state] += 1;
	  state_action_counts_[state][action] += 1;
	  qtable_[state][action] = w;

	  return v;
	}
	else {
	  state_counts_[state] = 1;
	  return Rollout(state, depth);
	}
}

double UCTAgent::Rollout(State state, int depth)
{
  double sum = 0.0;
  while (depth < 1.0 / (1.0 - gamma) && !state.terminated()) {
    Action action = Action(rand() % ActionSize);
    std::pair<State, double> sample = TaxiEnv::Sample(state, action);
    state = sample.first;
    sum += sample.second;
    depth += 1;
  }

  return sum;
}

double UCTAgent::Confidence(int t, int n)
{
	if (n > 0) {
		return sqrt(2.0 * log(t) / n);
	}
	else {
		return 1.0e6;
	}
}

