//
// Created by baj on 2/2/17.
//

#include "HierarchicalAgent.h"

const double HierarchicalAgent::alpha = 0.125;
const double HierarchicalAgent::gamma = 1.0;

void HierarchicalAgent::setEnv_(TaxiEnv *env_) {
  HierarchicalAgent::env_ = env_;
}

void HierarchicalAgent::reset() {
  rewards = 0.0;
  steps = 0;
}

int HierarchicalAgent::getMax_steps() const {
  return max_steps;
}

void HierarchicalAgent::setMax_steps(int max_steps) {
  HierarchicalAgent::max_steps = max_steps;
}

void HierarchicalAgent::setVerbose(bool verbose) {
  HierarchicalAgent::verbose = verbose;
}

void HierarchicalAgent::setParams(const unordered_map<string, int> &params) {
  HierarchicalAgent::parameters = params;
}
