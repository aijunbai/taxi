//
// Created by baj on 1/14/17.
//

#ifndef MAXQ_OP_HIERARCHICALFSM_H
#define MAXQ_OP_HIERARCHICALFSM_H

#include <string>
#include <stack>
#include <vector>
#include "state.h"
#include "HierarchicalFSMAgent.h"

using namespace std;

namespace fsm {
class HierarchicalFSM;
}

namespace std {

const string &to_string(fsm::HierarchicalFSM *m);

}

namespace fsm {

template<class T>
class ChoicePoint;

/**
 * hierarchical finite state machines
 */
class HierarchicalFSM {
  template<class T> friend
  class ChoicePoint;

  template<class T> friend
  class MakeChoice;

  friend class Runner;

  friend class Actuator;

public:
  HierarchicalFSM(HierarchicalFSMAgent *p, const std::string &name);

  virtual ~HierarchicalFSM();

  const State &state() { return agent->env()->state(); }

  string machineState() { return agent->getMachineState(); }

private:
  virtual void run(unordered_map<string, int> parameters = {}) = 0;

protected:
  /**
   * send commands to env, and wait for new info
   */
  void action(Action a);

  bool running();

  std::string getStackStr();

public:
  HierarchicalFSMAgent *agent;
  const std::string name;

public:
  const std::string &getName() const;
};

class Root: public HierarchicalFSM {
public:
  Root(HierarchicalFSMAgent *p);
  virtual ~Root();
  virtual void run(unordered_map<string, int> parameters = {});

private:
  HierarchicalFSM *get;
  HierarchicalFSM *put;
};

class Primitive: public HierarchicalFSM {
public:
  Primitive(HierarchicalFSMAgent *p, Action a, string name);
  virtual ~Primitive();
  virtual void run(unordered_map<string, int> parameters = {});

private:
  Action action;
};

class Get: public HierarchicalFSM {
public:
  Get(HierarchicalFSMAgent *p);
  virtual ~Get();
  virtual void run(unordered_map<string, int> parameters = {});

private:
  HierarchicalFSM *pickup;
  HierarchicalFSM *nav;
};

class Put: public HierarchicalFSM {
public:
  Put(HierarchicalFSMAgent *p);
  virtual ~Put();
  virtual void run(unordered_map<string, int> parameters = {});

private:
  HierarchicalFSM *putdown;
  HierarchicalFSM *nav;
};

class Navigate: public HierarchicalFSM {
public:
  Navigate(HierarchicalFSMAgent *p);
  virtual ~Navigate();
  virtual void run(unordered_map<string, int> parameters = {});

private:
  HierarchicalFSM *north;
  HierarchicalFSM *south;
  HierarchicalFSM *east;
  HierarchicalFSM *west;

  ChoicePoint<HierarchicalFSM *> *choice;
  ChoicePoint<int> *choice2;
};


}

#endif //MAXQ_OP_HIERARCHICALFSM_H
