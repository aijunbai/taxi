//
// Created by baj on 2/2/17.
//

#include "MaxQ0Agent.h"

MaxQ0Agent::MaxQ0Agent(const bool test): HierarchicalAgent(test) {
  reset();

  subtasks_[Root_T] = {Get_T, Put_T};

  subtasks_[Get_T] = {Pickup_T, NavB_T, NavG_T, NavR_T, NavY_T};
  subtasks_[Put_T] = {Putdown_T, NavB_T, NavG_T, NavR_T, NavY_T};

  subtasks_[NavB_T] = {North_T, South_T, East_T, West_T};
  subtasks_[NavG_T] = {North_T, South_T, East_T, West_T};
  subtasks_[NavR_T] = {North_T, South_T, East_T, West_T};
  subtasks_[NavY_T] = {North_T, South_T, East_T, West_T};
}

double MaxQ0Agent::run() {
  State state = env()->state();
  MaxQ0(Root_T, state);
  return rewards;
}

MaxQ0Agent::Task MaxQ0Agent::Pi(Task i, const State &s)
{
  std::vector<double> distri(subtasks_[i].size(), -1000000.0);
  for (uint j = 0; j < subtasks_[i].size(); ++j) {
    Task a = Task(subtasks_[i][j]);
    if (IsActiveState(a, s)) {
      distri[j] = Q(i, s, a);
    }
  }

  int best = PolicyFactory::instance().CreatePolicy(test()? PT_Greedy: PT_EpsilonGreedy)->get_action(distri);
  return (Task) subtasks_[i][best];
}

MaxQ0Agent::Task MaxQ0Agent::argmaxQ(Task i, const State &s) {
  std::vector<double> distri(subtasks_[i].size(), -1000000.0);
  for (uint j = 0; j < subtasks_[i].size(); ++j) {
    Task a = Task(subtasks_[i][j]);
    if (IsActiveState(a, s)) {
      distri[j] = Q(i, s, a);
    }
  }

  int best = PolicyFactory::instance().CreatePolicy(PT_Greedy)->get_action(distri);
  return (Task) subtasks_[i][best];
}

double MaxQ0Agent::Q(Task i, const State &s, Task a)
{
  return V(a, s) + ctable_[i][s][a];
}

int MaxQ0Agent::MaxQ0(Task i, State s)
{
  if (IsPrimitive(i)) {
    Action action = TaskToAction(i);
    double reward = env()->step(action);
    vtable_[i][s] = (1.0 - alpha) * vtable_[i][s] + alpha * reward;
    inc(reward);
    return 1;
  }
  else {
    int count = 0;
    while (!IsTerminalState(i, s) && steps < max_steps) {
      Task a = Pi(i, s);
      int N = MaxQ0(a, s);
      State s_prime = env()->state();
      double r = pow(gamma, N) * V(i, s_prime);
      ctable_[i][s][a] = (1 - alpha) * ctable_[i][s][a] + alpha * r;
      count += N;
      s = s_prime;
    }
    return count;
  }
}

double MaxQ0Agent::V(Task i, const State &s)
{
  if (IsPrimitive(i)) {
    return vtable_[i][s];
  }
  else {
    return Q(i, s, argmaxQ(i, s));
  }
};

bool MaxQ0Agent::IsPrimitive(Task task)
{
  return task == Pickup_T ||
         task == Putdown_T ||
         task == North_T ||
         task == South_T ||
         task == West_T ||
         task == East_T;
}

bool MaxQ0Agent::IsActiveState(Task task, const State & state)
{
  switch (task) {
    case Root_T:
      return !state.terminated();

    case Get_T: return !state.loaded();
    case Put_T: return state.loaded();

    case NavR_T:
      return state.taxiPosition() != TaxiEnv::Model::ins().terminals()[1];
    case NavY_T:
      return state.taxiPosition() != TaxiEnv::Model::ins().terminals()[0];
    case NavG_T:
      return state.taxiPosition() != TaxiEnv::Model::ins().terminals()[3];
    case NavB_T:
      return state.taxiPosition() != TaxiEnv::Model::ins().terminals()[2];

    case Pickup_T:
    case Putdown_T:
    case North_T:
    case South_T:
    case East_T:
    case West_T:
      return true; //can be performed in any state

    default: assert(0); return false;
  }
}

bool MaxQ0Agent::IsTerminalState(Task task, const State & state)
{
  if (state.terminated()) return true;

  switch (task) {
    case Root_T:
      return state.terminated();

    case Get_T: return state.loaded();
    case Put_T: return state.unloaded();

    case NavR_T: return state.taxiPosition() == TaxiEnv::Model::ins().terminals()[1];
    case NavY_T: return state.taxiPosition() == TaxiEnv::Model::ins().terminals()[0];
    case NavG_T: return state.taxiPosition() == TaxiEnv::Model::ins().terminals()[3];
    case NavB_T: return state.taxiPosition() == TaxiEnv::Model::ins().terminals()[2];

    case Pickup_T:
    case Putdown_T:
    case North_T:
    case South_T:
    case East_T:
    case West_T: return true; //terminate in any state

    default: PRINT_VALUE(task); assert(0); return false;
  }
}

