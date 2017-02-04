//
// Created by baj on 2/2/17.
//

#ifndef MAXQ_OP_HIERARCHICALAGENT_H
#define MAXQ_OP_HIERARCHICALAGENT_H

#include "agent.h"

class HierarchicalAgent: public Agent {
public:
  HierarchicalAgent(bool test): Agent(test) {
    max_steps = 1024;
    verbose = false;
  }

  virtual ~HierarchicalAgent() {

  }

  static const double alpha;
  static const double gamma;

  TaxiEnv *env() { return env_; }

  TaxiEnv *env_;
  bool verbose;
  int rewards;
  int steps;
  int max_steps;
  unordered_map<string, int> parameters;

  void setParams(const unordered_map<string, int> &params);

  void setMax_steps(int max_steps);

  int getMax_steps() const;

  void setVerbose(bool verbose);

  void setEnv_(TaxiEnv *env_);

  virtual void reset();

  virtual double run() = 0;

  virtual string name() = 0;

  void inc(double reward) {
    rewards += pow(gamma, steps) * reward;
    steps += 1;
  }

  // not used
  virtual Action plan(const State & state) { return Null; }
  virtual void learn(const State &, int, double, const State &, int) { }
  virtual void terminate(const State &, int, double) { }
};


#endif //MAXQ_OP_HIERARCHICALAGENT_H
