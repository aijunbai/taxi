//
// Created by baj on 2/3/17.
//

#include "MaxQQAgent.h"

MaxQQAgent::MaxQQAgent(const bool test): MaxQ0Agent(test) {

}

list<State> MaxQQAgent::MaxQQ(Task i, State s)
{
  if (IsPrimitive(i)) {
    Action action = TaskToAction(i);
    double reward = env()->step(action);
    vtable_[i][s] = (1.0 - alpha) * vtable_[i][s] + alpha * reward;
    inc(reward);
    return {s};
  }
  else {
    list<State> seq;
    while (!IsTerminalState(i, s) && steps < max_steps) {
      Task a = Pi2(i, s);
      auto childSeq = MaxQQ(a, s);
      State s_prime = env()->state();
      Task a_star = argmaxQ2(i, s_prime);
      int N = childSeq.size();
      double d = pow(gamma, N);
      double r = d * Q(i, s_prime, a_star);
      double r2 = d * (R2(i, s_prime) + Q2(i, s_prime, a_star)); //pseudo Q
      for (auto &e : childSeq) {
        ctable_[i][e][a] = (1 - alpha) * ctable_[i][e][a] + alpha * r;
        ctable2_[i][e][a] = (1 - alpha) * ctable2_[i][e][a] + alpha * r2;
        N -= 1;
      }

      seq.splice(seq.begin(), childSeq);
      s = s_prime;
    }

    return seq;
  }
}

double MaxQQAgent::run() {
  State state = env()->state();
  buildHierarchy(state);
  MaxQQ(Root_T, state);
  return rewards;
}

double MaxQQAgent::R2(Task i, const State &s)
{
  if (IsTerminalState(i, s)) return 10.0;
  return 0.0;
}

MaxQ0Agent::Task MaxQQAgent::Pi2(Task i, const State &s)
{
  std::vector<double> distri(subtasks_[i].size(), -1000000.0);
  for (uint j = 0; j < subtasks_[i].size(); ++j) {
    Task a = Task(subtasks_[i][j]);
    if (IsActiveState(a, s)) {
      distri[j] = Q2(i, s, a);
    }
  }

  int best = PolicyFactory::instance().CreatePolicy(test()? PT_Greedy: PT_EpsilonGreedy)->get_action(distri);
  return (Task) subtasks_[i][best];
}

MaxQQAgent::Task MaxQQAgent::argmaxQ2(Task i, const State &s) {
  std::vector<double> distri(subtasks_[i].size(), -1000000.0);
  for (uint j = 0; j < subtasks_[i].size(); ++j) {
    Task a = Task(subtasks_[i][j]);
    if (IsActiveState(a, s)) {
      distri[j] = Q2(i, s, a);
    }
  }

  int best = PolicyFactory::instance().CreatePolicy(PT_Greedy)->get_action(distri);
  return (Task) subtasks_[i][best];
}

double MaxQQAgent::Q2(Task i, const State &s, Task a)
{
  return V(a, s) + ctable2_[i][s][a];
}