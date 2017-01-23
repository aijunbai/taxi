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

  const State &state() { return agent->env()->get_state(); }

  string machineState() { return agent->getMachineState(); }

private:
  virtual void run(const vector<string> &parameters = {}) = 0;

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
  virtual void run(const vector<string> & parameters = {});

private:
  HierarchicalFSM *get;
  HierarchicalFSM *put;
  HierarchicalFSM *refuel;

  ChoicePoint<HierarchicalFSM *> *choice;
};

class Primitive: public HierarchicalFSM {
public:
  Primitive(HierarchicalFSMAgent *p, Action a, string name);
  virtual ~Primitive();
  virtual void run(const vector<string> & parameters = {});

private:
  Action action;
};

class Get: public HierarchicalFSM {
public:
  Get(HierarchicalFSMAgent *p);
  virtual ~Get();
  virtual void run(const vector<string> & parameters = {});

private:
  HierarchicalFSM *pickup;
  HierarchicalFSM *nav;

  ChoicePoint<HierarchicalFSM *> *choice;
};

class Refuel: public HierarchicalFSM {
public:
  Refuel(HierarchicalFSMAgent *p);
  virtual ~Refuel();
  virtual void run(const vector<string> & parameters = {});

private:
  HierarchicalFSM *fillup;
  HierarchicalFSM *nav;

  ChoicePoint<HierarchicalFSM *> *choice;
};

class Put: public HierarchicalFSM {
public:
  Put(HierarchicalFSMAgent *p);
  virtual ~Put();
  virtual void run(const vector<string> &parameters = {});

private:
  HierarchicalFSM *putdown;
  HierarchicalFSM *nav;

  ChoicePoint<HierarchicalFSM *> *choice;
};

class Navigate: public HierarchicalFSM {
public:
  Navigate(HierarchicalFSMAgent *p);
  virtual ~Navigate();
  virtual void run(const vector<string> & parameters = {});

private:
  HierarchicalFSM *north;
  HierarchicalFSM *south;
  HierarchicalFSM *east;
  HierarchicalFSM *west;

  ChoicePoint<HierarchicalFSM *> *dir_choice;
  ChoicePoint<int> *step_choice;
};


}

#endif //MAXQ_OP_HIERARCHICALFSM_H
