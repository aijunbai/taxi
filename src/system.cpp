/*
 * system.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#include <iostream>
#include <cassert>

#include "state.h"
#include "system.h"
#include "agent.h"

using namespace std;

double System::simulate(Agent & agent, bool verbose)
{
	const int max_steps = 1024;

	env_.reset();

	State state = env_.get_state();
	Action action = agent.plan(state);

	int step = 0;
	double rewards = 0.0;

	do {
		step += 1;

		if (verbose) {
			cout << "Step " << step << " | State: " << env_.get_state() << " | Action: " << action_name(Action(action));
		}

		double reward = env_.step(action); //taking action
		rewards += reward;

		if (env_.terminate() || step >= max_steps) {
			if (!agent.test()) {
				agent.terminate(state, action, reward);
			}

			if (verbose) {
				cout << " | Reward: " << reward;

				if (env_.unloaded()) {
					cout << " | Success" << endl;
				}
				else {
					cout << " | Failure" << endl;
				}
			}
			step += 1;
			break;
		}

		State post_state = env_.get_state(); //observing s'
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
		cout << "Step " << step << " | State: " << env_.get_state() << std::endl;
	}

	return rewards;
}

double System::simulateFSM(HierarchicalFSMAgent & agent, bool verbose, bool useStaticTransition)
{
  const int max_steps = 1024;

  agent.setEnv(&env_);
  agent.setMax_steps(max_steps);
  agent.setVerbose(verbose);
  agent.setUseStaticTransition(useStaticTransition);

  agent.reset();
  env_.reset();

  double rewards = agent.run();
  return rewards;
}
