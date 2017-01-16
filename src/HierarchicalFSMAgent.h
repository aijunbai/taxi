//
// Created by baj on 1/14/17.
//

#ifndef MAXQ_OP_HIERARCHICALFSMAGENT_H
#define MAXQ_OP_HIERARCHICALFSMAGENT_H

#include "agent.h"

using namespace std;

class HierarchicalFSMAgent: public Agent {
public:
  HierarchicalFSMAgent(const bool test): Agent(test), name_("hierarchicalfsm") {
    max_steps = 1024;
    verbose = false;

    reset();
  }

  virtual ~HierarchicalFSMAgent() {
    if (!test()) {
      for (auto &pa : qtable_) {
        pa.second.save(name_ + "_" + pa.first);
      }
    }
  }

  void setMax_steps(int max_steps);

  void setVerbose(bool verbose);

  virtual Action plan(const State & state) { return Nil; }
  virtual void learn(const State &, int, double, const State &, int) { }
  virtual void terminate(const State &, int, double) { }

  TaxiEnv *env() { return env_; }

  double run();

  void reset();

  void setEnv(TaxiEnv *e) { env_ = e; }

  void step(Action a);

  bool running();

public:
  TaxiEnv *env_;
  vector<string> stack;
  State lastState;
  int lastChoice;
  string lastMachineState;
  unordered_map<string, StateActionPairTable<double>> qtable_;

public:
  int max_steps;
  bool verbose;

  double epsilon;
  double alpha;
  double rewards;
  double gamma;
  double discount;
  int steps;
  double accumulatedRewards;
  double accumulatedDiscount;

  const std::string name_;

public:
  string getMachineState() const {
    return to_prettystring(stack);
  }

  double & Q(const State &state, const string &machineState, int action) {
    return qtable_[machineState].operator()(state, action);
  }

  double & V(const State &state, const string &machineState, int numChoices);

  int selectChoice(const State &state, const string &machineState, int numChoices);
  int argmaxQ(const State &state, const string &machineState, int numChoices);

  int Qupdate(const State &state, const string &machineState, int numChoices);

public:
  void PushStack(const string &s);
  void PopStack();
};


#endif //MAXQ_OP_HIERARCHICALFSMAGENT_H
