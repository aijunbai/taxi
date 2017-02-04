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
    double reward = env_->step(action);
    vtable_[i][s] = (1.0 - alpha) * vtable_[i][s] + alpha * reward;
    rewards += reward;
    steps += 1;
    return {s};
  }
  else {
    list<State> seq;
    while (!IsTerminalState(i, s) && steps < max_steps) {
      Task a = Pi(i, s);
      auto childSeq = MaxQQ(a, s);
      State s_prime = env()->state();
      Task a_star = argmaxQ(i, s_prime);
      int N = childSeq.size();
      for (auto &e : childSeq) {
        ctable_[i][e][a] = (1 - alpha) * ctable_[i][e][a] + alpha * pow(gamma, N) * Q(i, s_prime, a_star);
        N -= 1;
      }

      seq.splice(seq.begin(), childSeq);
      s = s_prime;
    }

    return seq;
  }
}

double MaxQQAgent::run() {
  State state = env_->state();
  MaxQQ(Root_T, state);
  return rewards;
}