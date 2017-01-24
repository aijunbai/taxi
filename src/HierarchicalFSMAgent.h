//
// Created by baj on 1/14/17.
//

#ifndef MAXQ_OP_HIERARCHICALFSMAGENT_H
#define MAXQ_OP_HIERARCHICALFSMAGENT_H

#include "agent.h"
#include "dot_graph.h"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/algorithm/string/replace.hpp>


using namespace std;

class HierarchicalFSMAgent: public Agent {
public:
  HierarchicalFSMAgent(const bool test);

  virtual ~HierarchicalFSMAgent();

  string name() {
    string name = "hierarchicalfsm";
    if (useStaticTransition) name += "-det";
    return name;
  }

  void setUseStaticTransition(bool useStaticTransition);

  bool isUseStaticTransition() const;

  void saveStaticTransitions(string filename);

  void loadStaticTransitions(string filename);

  void setMax_steps(int max_steps);

  void setVerbose(bool verbose);

  virtual Action plan(const State & state) { return Null; }
  virtual void learn(const State &, int, double, const State &, int) { }
  virtual void terminate(const State &, int, double) { }

  TaxiEnv *env() { return env_; }

  double run();

  void reset();

  void setEnv(TaxiEnv *e) { env_ = e; }

  void step(Action a);

  bool running();

  void showHistory();

public:
  TaxiEnv *env_;
  vector<string> stack;
  State lastState;
  int lastChoice;
  int lastChoiceTime;
  string lastMachineState;
  HashMap<State, HashMap<string, HashMap<int, double>>> qtable_;

  vector<tuple<State, string, Action, double>> history;

public:
  int max_steps;
  bool verbose;
  bool useStaticTransition;

  double epsilon;
  double alpha;
  double rewards;
  double gamma;
  double discount;
  int steps;
  double accumulatedRewards;
  double accumulatedDiscount;

public:
  string getMachineState() const {
    return to_prettystring(stack);
  }

  double & Q(const State &state, const string &machineState, int choice);

  double & V(const State &state, const string &machineState, int numChoices);

  int selectChoice(const State &state, const string &machineState, int numChoices);
  int argmaxQ(const State &state, const string &machineState, int numChoices);

  int Qupdate(const State &state, const string &machineState, int numChoices, int current_time);

public:
  void PushStack(const string &s);
  void PopStack();

private:
  HashMap<State, HashMap<string, \
    HashMap<int, HashMap<State, \
        HashMap<string, double>>>>> staticTransition;
  HashMap<string, int> numChoicesMap;

public:
  bool isStaticTransition(const State &state, const string &machine_state, int c);
};

#endif //MAXQ_OP_HIERARCHICALFSMAGENT_H
