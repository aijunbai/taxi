//
// Created by baj on 1/14/17.
//

#ifndef MAXQ_OP_CHECKPOINT_H
#define MAXQ_OP_CHECKPOINT_H


#include "HierarchicalFSM.h"
#include <cstring>
#include <functional>
#include <string>
#include <vector>

namespace fsm {

template<class T>
class ChoicePoint {
public:
  ChoicePoint(const std::string &name, const std::vector<T> c)
      : name(name), choices(c) {}

  const string &getName() const { return name; }

  T choose(HierarchicalFSM *m) {
    auto state = m->state();
    auto machineState = m->machineState();
    auto i = m->agent->Qupdate(state, machineState, choices.size(), m->agent->steps);
    assert(i < (int) choices.size());
    return choices[i];
  }

private:
  std::string name;
  std::vector<T> choices;
};

/**
 * Make choice while taking care of call stack
 */
template<class T>
class MakeChoice {
public:
  MakeChoice(HierarchicalFSM *m, ChoicePoint<T> *cp) : m(m), cp(cp) {
    m->agent->PushStack(cp->getName());
  }

  T operator()() {
    auto c = cp->choose(m);
    m->agent->PushStack("[" + to_string(c) + "]");
    return c;
  }

  ~MakeChoice() {
    m->agent->PopStack();
    m->agent->PopStack();
  }

private:
  HierarchicalFSM *m;
  ChoicePoint<T> *cp;
};

/**
 * run a child machine while taking care of call stack
 */
class Runner {
public:
  Runner(HierarchicalFSM *m, const vector<string> &parameters = {}) : m(m), parameters(parameters) {
    if (parameters.size())
      m->agent->PushStack(m->getName() + " (" + to_prettystring(parameters) + ")");
    else
      m->agent->PushStack(m->getName());
  }

  void operator()() { m->run(parameters); }

  ~Runner() { m->agent->PopStack(); }

private:
  HierarchicalFSM *m;
  vector<string> parameters;
};

class Actuator {
public:
  Actuator(HierarchicalFSM *m, vector<string> parameters = {}) : m(m) {
    m->agent->PushStack("!" + to_prettystring(parameters));
  }

  void operator()(Action a) { m->action(a); }

  ~Actuator() { m->agent->PopStack(); }

private:
  HierarchicalFSM *m;
};

}



#endif //MAXQ_OP_CHECKPOINT_H
