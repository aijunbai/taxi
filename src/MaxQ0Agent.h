//
// Created by baj on 2/2/17.
//

#ifndef MAXQ_OP_MAXQ0AGENT_H
#define MAXQ_OP_MAXQ0AGENT_H

#include "HierarchicalAgent.h"
#include "table.h"
#include "state.h"

using namespace std;

class MaxQ0Agent: public HierarchicalAgent {
public:
  enum Task {
    Root_T = 0,

    Get_T,
    Refuel_T,
    Put_T,

    NavR_T,
    NavY_T,
    NavB_T,
    NavG_T,
    NavF_T,

    Pickup_T,
    Putdown_T,
    North_T,
    South_T,
    East_T,
    West_T,
    Fillup_T,

    TaskSize
  };

  static Action TaskToAction(Task i) {
    switch (i) {
      case Pickup_T: return Pickup;
      case Putdown_T: return Putdown;
      case North_T: return North;
      case South_T: return South;
      case East_T: return East;
      case West_T: return West;
      case Fillup_T: return Fillup;
      default: assert(0); return Null;
    }
  }

public:
  MaxQ0Agent(const bool test);
  virtual ~MaxQ0Agent() { }

  string name() {
    return "MaxQ0";
  }

  virtual double run();

  int MaxQ0(Task i, State s);
  pair<double, Task> EvaluateMaxNode(Task i, const State &s);
  Task Pi(Task i, const State &s);
  double Q(Task i, const State &s, Task a);

  bool IsActiveState(Task task, const State & state);
  bool IsTerminalState(Task task, const State & state);

  bool IsPrimitive(Task task);

  unordered_map<int, unordered_map<State, double>> vtable_;
  unordered_map<int, unordered_map<State, unordered_map<int, double>>> ctable_;
  unordered_map<int, unordered_set<int>> subtasks_;

  Task argmaxQ(Task i, const State &state);
};


#endif //MAXQ_OP_MAXQ0AGENT_H
