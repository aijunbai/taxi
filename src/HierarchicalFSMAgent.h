//
// Created by baj on 1/14/17.
//

#ifndef MAXQ_OP_HIERARCHICALFSMAGENT_H
#define MAXQ_OP_HIERARCHICALFSMAGENT_H

#include "HierarchicalAgent.h"
#include "dot_graph.h"

#include <boost/algorithm/string/replace.hpp>

using namespace std;

class HierarchicalFSMAgent: public HierarchicalAgent {
public:
  HierarchicalFSMAgent(const bool test);

  virtual ~HierarchicalFSMAgent();

  string name() {
    string name = "hierarchicalfsm";
    if (leverageInternalTransitions()) name += "-int";
    return name;
  }

  virtual double run();

  virtual void reset();

  void step(Action a);

  bool running();

//  void showHistory();

public:
  vector<string> stack;
//  vector<tuple<State, string, Action, double>> history;

  State lastState;
  int lastChoice;
  int lastChoiceTime;
  string lastMachineState;
  HashMap<State, HashMap<string, HashMap<int, double>>> qtable_;

public:
  bool leverageInternalTransitions() {
    static bool ret = parameters["leverageInternalTransitions"];
    return ret;
  }

  double accumulatedRewards;
  double accumulatedDiscount;

public:
  string getMachineState() const {
    return to_prettystring(stack);
  }

  double & Q(const State &state, const string &machineState, int choice);

  double V(const State &state, const string &machineState, int numChoices);

  int selectChoice(const State &state, const string &machineState, int numChoices);
  int argmaxQ(const State &state, const string &machineState, int numChoices);

  int QUpdate(const State &state, const string &machineState, int numChoices, int current_time);

public:
  void PushStack(const string &s);
  void PopStack();

private:
  unordered_map<string, int> numChoicesMap;
  unordered_map<TaxiEnv::cond_t, \
  unordered_map<string, \
    unordered_map<int, \
        unordered_map<string, double>>>> internalTransitions;

public:
  bool isStaticTransition(const State &state, const string &machine_state, int c);
};

#endif //MAXQ_OP_HIERARCHICALFSMAGENT_H
