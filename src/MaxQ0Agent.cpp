//
// Created by baj on 2/2/17.
//

#include <climits>
#include "MaxQ0Agent.h"

MaxQ0Agent::MaxQ0Agent(const bool test): HierarchicalAgent(test) {
  reset();

  subtasks_[Root_T] = {Get_T, Put_T, Refuel_T};

  subtasks_[Get_T] = {Pickup_T, NavB_T, NavG_T, NavR_T, NavY_T};
  subtasks_[Refuel_T] = {Fillup_T, NavF_T};
  subtasks_[Put_T] = {Putdown_T, NavB_T, NavG_T, NavR_T, NavY_T};

  subtasks_[NavB_T] = {North_T, South_T, East_T, West_T};
  subtasks_[NavG_T] = {North_T, South_T, East_T, West_T};
  subtasks_[NavR_T] = {North_T, South_T, East_T, West_T};
  subtasks_[NavY_T] = {North_T, South_T, East_T, West_T};
  subtasks_[NavF_T] = {North_T, South_T, East_T, West_T};
}

double MaxQ0Agent::run() {
  State state = env_->state();
  MaxQ0(Root_T, state);
  return rewards;
}

MaxQ0Agent::Task MaxQ0Agent::Pi(Task i, const State &s)
{
  if (!test() && drand48() < epsilon) {
    vector<int> actions(subtasks_[int(i)].begin(), subtasks_[int(i)].end());
    return Task(actions[rand() % actions.size()]);
  }
  else {
    return argmaxQ(i, s);
  }
}

MaxQ0Agent::Task MaxQ0Agent::argmaxQ(Task i, const State &state) {
  Task bestAction = TaskSize;
  double bestValue = -100000.0;
  int numTies = 0;

  for (auto a : subtasks_[i]) {
    double value = Q(i, state, Task(a));
    if (value > bestValue) {
      bestValue = value;
      bestAction = Task(a);
    } else if (value == bestValue) {
      numTies++;
      if (rand() % (numTies + 1) == 0) {
        bestValue = value;
        bestAction = Task(a);
      }
    }
  }

  assert(bestAction != TaskSize);
  return bestAction;
}

double MaxQ0Agent::Q(Task i, const State &s, Task a)
{
  return EvaluateMaxNode(a, s).first + ctable_[i][s][a];
}

int MaxQ0Agent::MaxQ0(Task i, State s)
{
  if (IsPrimitive(i)) {
    Action action = TaskToAction(i);
    double reward = env_->step(action);
    vtable_[i][s] = (1.0 - alpha) * vtable_[i][s] + alpha * reward;
    rewards += reward;
    steps += 1;
    return 1;
  }
  else {
    int count = 0;
    while (!IsTerminalState(i, s) && steps < max_steps) {
      Task a = Pi(i, s);
      int N = MaxQ0(a, s);
      State s_prime = env_->state();
      ctable_[i][s][a] = (1 - alpha) * ctable_[i][s][a] + alpha * pow(gamma, N) * EvaluateMaxNode(i, s_prime).first;
      count += N;
      s = s_prime;
    }
    return count;
  }
}

pair<double, MaxQ0Agent::Task> MaxQ0Agent::EvaluateMaxNode(Task i, const State &s)
{
  if (IsPrimitive(i)) {
    return {vtable_[i][s], i};
  }
  else {
    pair<double, Task> best(-100000.0, TaskSize);
    for (auto j : subtasks_[i]) {
      auto q = Q(i, s, Task(j));
      if (q > best.first) {
        best.first = q;
        best.second = Task(j);
      }
    }

    return best;
  }
};

bool MaxQ0Agent::IsPrimitive(Task task)
{
  return task == Pickup_T ||
         task == Putdown_T ||
         task == North_T ||
         task == South_T ||
         task == West_T ||
         task == East_T ||
         task == Fillup_T;
}

bool MaxQ0Agent::IsActiveState(Task task, const State & state)
{
  switch (task) {
    case Root_T: return true;

    case Get_T: return state.passenger() != int(TaxiEnv::Model::ins().terminals().size());
    case Put_T: return state.passenger() == int(TaxiEnv::Model::ins().terminals().size());
    case Refuel_T: return !state.refueled();

    case NavR_T:
    case NavY_T:
    case NavG_T:
    case NavB_T: return true;

    case Pickup_T:
    case Putdown_T:
    case North_T:
    case South_T:
    case East_T:
    case West_T: return true; //can performed in any state

    default: assert(0); return false;
  }
}

bool MaxQ0Agent::IsTerminalState(Task task, const State & state)
{
  if (state.terminated()) return true;

  switch (task) {
    case Root_T: return state.unloaded();

    case Get_T: return state.loaded();
    case Put_T: return state.unloaded();
    case Refuel_T: return state.refueled();

    case NavR_T: return state.taxiPosition() == TaxiEnv::Model::ins().terminals()[1];
    case NavY_T: return state.taxiPosition() == TaxiEnv::Model::ins().terminals()[0];
    case NavG_T: return state.taxiPosition() == TaxiEnv::Model::ins().terminals()[3];
    case NavB_T: return state.taxiPosition() == TaxiEnv::Model::ins().terminals()[2];
    case NavF_T: return state.taxiPosition() == TaxiEnv::Model::ins().fuelPosition();

    case Pickup_T:
    case Putdown_T:
    case North_T:
    case South_T:
    case East_T:
    case West_T: return true; //terminate in any state

    default: PRINT_VALUE(task); assert(0); return false;
  }
}

