//
// Created by baj on 2/2/17.
//

#include "HierarchicalAgent.h"

void HierarchicalAgent::setEnv_(TaxiEnv *env_) {
  HierarchicalAgent::env_ = env_;
}

TaxiEnv *HierarchicalAgent::getEnv_() const {
  return env_;
}

void HierarchicalAgent::reset() {
  epsilon = 0.01;
  alpha = 0.125;
  gamma = 1.0;

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
