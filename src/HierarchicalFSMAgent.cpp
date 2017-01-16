//
// Created by baj on 1/14/17.
//

#include <climits>
#include "HierarchicalFSMAgent.h"
#include "HierarchicalFSM.h"
#include "ChoicePoint.h"

using namespace fsm;

void HierarchicalFSMAgent::step(Action a) {
  if (verbose) {
    cout << "Step " << steps << " | State: " << env_->get_state()
        << " x MachineState: " << getMachineState()
         << " | Action: " << action_name(Action(a));
  }

  double reward = env()->step(a);
  rewards += reward;
  steps += 1;

  accumulatedRewards += accumulatedDiscount * reward;
  accumulatedDiscount *= gamma;

  if (verbose) {
    cout << " | Reward: " << reward << endl;
  }
}

bool HierarchicalFSMAgent::running() {
  bool ret = !env()->terminate() && steps < max_steps;

  if (!ret) {
    if (verbose) {
      if (env()->unloaded()) {
        cout << " | Success" << endl;
      }
      else {
        cout << " | Failure" << endl;
      }
    }
  }

  return ret;
}

void HierarchicalFSMAgent::reset() {
  epsilon = 0.01;
  alpha = 0.125;
  rewards = 0.0;
  gamma = 0.95;
  steps = 0;
  accumulatedRewards = 0.0;
  accumulatedDiscount = 1.0;

  lastState = {0, 0, 0, 0};
  lastChoice = 0;
  stack.clear();
  lastMachineState.clear();
}

void HierarchicalFSMAgent::PushStack(const string &s) {
  stack.push_back(s);
}

void HierarchicalFSMAgent::PopStack() {
  stack.pop_back();
}

double HierarchicalFSMAgent::run() {
  HierarchicalFSM *machine = new Root(this);
  Runner(machine).operator()();
  delete machine;
  return rewards;
}

void HierarchicalFSMAgent::setVerbose(bool verbose) {
  HierarchicalFSMAgent::verbose = verbose;
}

void HierarchicalFSMAgent::setMax_steps(int max_steps) {
  HierarchicalFSMAgent::max_steps = max_steps;
}

int HierarchicalFSMAgent::selectChoice(const State &state, const string &machineState, int numChoices) {
  if (!test() && drand48() < epsilon) {
    return Action(rand() % numChoices);
  }
  else {
    return argmaxQ(state, machineState, numChoices);
  }
}

int HierarchicalFSMAgent::argmaxQ(const State &state, const string &machineState, int numChoices) {
  int bestAction = -1;
  double bestValue = (double) INT_MIN;
  int numTies = 0;

  for (int a = 0; a < numChoices; ++a) {
    double value = Q(state, machineState, a);
    if (value > bestValue) {
      bestValue = value;
      bestAction = a;
    } else if (value == bestValue) {
      numTies++;
      if (rand() % (numTies + 1) == 0) {
        bestValue = value;
        bestAction = a;
      }
    }
  }

  assert(bestAction != -1);
  return bestAction;
}

double & HierarchicalFSMAgent::V(const State &state, const string &machineState, int numChoices)
{
  return Q(state, machineState, argmaxQ(state, machineState, numChoices));
}

int HierarchicalFSMAgent::Qupdate(const State &state, const string &machineState, int numChoices)
{
  double &q = Q(lastState, lastMachineState, lastChoice);
  q += alpha * (accumulatedRewards + accumulatedDiscount * V(state, machineState, numChoices) - q);

  lastState = state;
  lastMachineState = machineState;
  accumulatedRewards = 0.0;
  accumulatedDiscount = 1.0;

  auto i = selectChoice(state, machineState, numChoices);
  lastChoice = i;
  return i;
}
