/*
 * system.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#include <iostream>
#include <cassert>
#include <unistd.h>
#include <cstdlib>

#include "state.h"
#include "system.h"
#include "agent.h"

using namespace std;

double System::simulate(Agent & agent, bool verbose)
{
	const int max_steps = 128;

	env_.reset();

	State state = env_.state();
	Action action = agent.plan(state);

	int step = 0;
	double rewards = 0.0;

	do {
		step += 1;

		if (verbose) {
			cout << "Step " << step << " | State: " << env_.state() << " | Action: " << action_name(Action(action));
		}

		double reward = env_.step(action); //taking action
		rewards += reward;

		if (env_.state().terminated() || step >= max_steps) {
			if (!agent.test()) {
				agent.terminate(state, action, reward);
			}

			if (verbose) {
				cout << " | Reward: " << reward;

				if (env_.state().unloaded()) {
					cout << " | Success" << endl;
				}
				else {
					cout << " | Failure" << endl;
				}
			}

			step += 1;
      break;
		}

		State post_state = env_.state(); //observing s'
		Action post_action = agent.plan(post_state); //choosing a'

		if (!agent.test()) {
			agent.learn(state, action, reward, post_state, post_action); //learning from experience
		}

		state = post_state;
		action = post_action;

		if (verbose) {
			cout << " | Reward: " << reward << endl;
		}
	} while(1);

	if (verbose) {
		cout << "Step " << step << " | State: " << env_.state() << std::endl;
	}

	return rewards;
}

double System::simulateHierarchicalAgent(HierarchicalAgent &agent, bool verbose,
																				 const unordered_map<string, int> &params)
{
  const int max_steps = 128;

  agent.setEnv_(&env_);
  agent.setMax_steps(max_steps);
  agent.setVerbose(verbose);
  agent.setParams(params);

  env_.reset();
  agent.reset();

  double rewards = agent.run();
  return rewards;
}
