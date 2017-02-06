//
// Created by baj on 2/3/17.
//

#ifndef MAXQ_OP_MAXQQAGENT_H
#define MAXQ_OP_MAXQQAGENT_H

#include "MaxQ0Agent.h"
#include <list>

class MaxQQAgent: public MaxQ0Agent {
public:
  MaxQQAgent(const bool test);

  string name() {
    return "MaxQQ";
  }

  virtual double run();

  list<State> MaxQQ(Task i, State s);

  double R2(Task i, const State &s);
  double Q2(Task i, const State &s, Task a);
  Task argmaxQ2(Task i, const State &s);
  Task Pi2(Task i, const State &s);

  unordered_map<int, unordered_map<State, unordered_map<int, double>>> ctable2_;
};


#endif //MAXQ_OP_MAXQQAGENT_H
