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

  virtual double run();

  list<State> MaxQQ(Task i, State s);
};


#endif //MAXQ_OP_MAXQQAGENT_H
