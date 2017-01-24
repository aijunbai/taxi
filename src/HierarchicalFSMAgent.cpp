//
// Created by baj on 1/14/17.
//

#include <climits>
#include "HierarchicalFSMAgent.h"
#include "HierarchicalFSM.h"
#include "ChoicePoint.h"

using namespace fsm;

HierarchicalFSMAgent::HierarchicalFSMAgent(const bool test): Agent(test) {
  max_steps = 1024;
  verbose = false;
  useStaticTransition = false;

  reset();

  if (useStaticTransition)
    loadStaticTransitions(name() + "_transitions.xml");
}

HierarchicalFSMAgent::~HierarchicalFSMAgent() {
  if (!test()) {
//    for (auto &pa : qtable_) {
//      pa.second.save(name() + "_" + pa.first);
//    }

//    if (useStaticTransition) {
//      saveStaticTransitions(name() + "_transitions.xml");
//
//      dot::Graph G;
//      for (auto &e : staticTransitions) {
//        auto s = to_prettystring(e.first);
//        boost::replace_all(s, ", {", ",\n{");
//        G.addNode(s, "red");
//        for (auto &o : e.second) {
//          auto a = to_string(o.first);
//          auto sa = s + ";  " + a;
//          G.addNode(sa, "gray");
//          G.addEdge(s, sa, "blue", a);
//          for (auto &x : o.second) {
//            auto s_ = to_prettystring(x.first);
//            boost::replace_all(s_, ", {", ",\n{");
//            auto n = to_string(x.second);
//            G.addNode(s_, "orange");
//            G.addEdge(sa, s_, "green", n);
//          }
//        }
//      }
//
//      G.dump(name() + "_transitions.dot");
//    }
  }
}

void HierarchicalFSMAgent::step(Action a) {
  auto state = env_->get_state();
  auto machineState = getMachineState();

  if (verbose) {
    cout << "Step " << steps << " | State: " << state
         << " x MachineState: " << machineState
         << " | Action: " << action_name(Action(a));
  }

  double reward = env()->step(a);
  rewards += pow(gamma, steps) * reward;
  steps += 1;

  accumulatedRewards += accumulatedDiscount * reward;
  accumulatedDiscount *= gamma;

  if (verbose) {
    cout << " | Reward: " << reward << endl;
  }

  history.push_back(make_tuple(state, machineState, a, reward));
}

void HierarchicalFSMAgent::showHistory()
{
  int t = 0;
  for (auto &e : history) {
    cout << "Step: " << t++ << " {{\n" << e << "\n}}\n" << endl;
  }
}

bool HierarchicalFSMAgent::running() {
  bool ret = !env()->terminated() && steps < max_steps;

  if (!ret) {
    if (verbose) {
      if (env()->unloaded()) {
        cout << " | Success" << endl;
      }
      else {
        cout << " | Failure" << endl;
      }
    }
  }

  return ret;
}

void HierarchicalFSMAgent::reset() {
  epsilon = 0.01;
  alpha = 0.125;
  rewards = 0.0;
  gamma = 1.0;
  steps = 0;
  accumulatedRewards = 0.0;
  accumulatedDiscount = 1.0;

  lastState = {0, 0, 0, 0, 0};
  lastChoice = 0;
  lastChoiceTime = -1;
  stack.clear();
  lastMachineState.clear();
  history.clear();
}

void HierarchicalFSMAgent::PushStack(const string &s) {
  stack.push_back(s);
}

void HierarchicalFSMAgent::PopStack() {
  stack.pop_back();
}

double HierarchicalFSMAgent::run() {
  HierarchicalFSM *machine = new Root(this);
  Runner(machine).operator()();
  delete machine;
  return rewards;
}

void HierarchicalFSMAgent::setVerbose(bool verbose) {
  HierarchicalFSMAgent::verbose = verbose;
}

void HierarchicalFSMAgent::setMax_steps(int max_steps) {
  HierarchicalFSMAgent::max_steps = max_steps;
}

int HierarchicalFSMAgent::selectChoice(const State &state, const string &machineState, int numChoices) {
  if (!test() && drand48() < epsilon) {
    return Action(rand() % numChoices);
  }
  else {
    return argmaxQ(state, machineState, numChoices);
  }
}

int HierarchicalFSMAgent::argmaxQ(const State &state, const string &machineState, int numChoices) {
  int bestAction = -1;
  double bestValue = (double) INT_MIN;
  int numTies = 0;

  for (int a = 0; a < numChoices; ++a) {
    double value = Q(state, machineState, a);
    if (value > bestValue) {
      bestValue = value;
      bestAction = a;
    } else if (value == bestValue) {
      numTies++;
      if (rand() % (numTies + 1) == 0) {
        bestValue = value;
        bestAction = a;
      }
    }
  }

  assert(bestAction != -1);
  return bestAction;
}

double & HierarchicalFSMAgent::V(const State &state, const string &machineState, int numChoices)
{
  return Q(state, machineState, argmaxQ(state, machineState, numChoices));
}

int HierarchicalFSMAgent::Qupdate(
    const State &state, const string &machineState, int numChoices, int current_time)
{
  assert(!numChoicesMap.count(machineState) || numChoicesMap[machineState] == numChoices);
  numChoicesMap[machineState] = numChoices;

  if (useStaticTransition &&
      current_time == lastChoiceTime && lastMachineState.size() &&
      machineState.size() && lastMachineState != machineState) {
    staticTransition[lastState][lastMachineState][lastChoice][state][machineState] += 1.0;
  }

  double &q = Q(lastState, lastMachineState, lastChoice);
  q += alpha * (accumulatedRewards + accumulatedDiscount * V(state, machineState, numChoices) - q);

  lastState = state;
  lastMachineState = machineState;
  accumulatedRewards = 0.0;
  accumulatedDiscount = 1.0;

  auto i = selectChoice(state, machineState, numChoices);
  lastChoice = i;
  lastChoiceTime = current_time;
  return i;
}

bool HierarchicalFSMAgent::isStaticTransition(
    const State &state, const string &machine_state, int c) {
  return staticTransition.count(state) &&
         staticTransition[state].count(machine_state) &&
         staticTransition[state][machine_state].count(c) &&
         staticTransition[state][machine_state][c].size();
}

bool HierarchicalFSMAgent::isUseStaticTransition() const {
  return useStaticTransition;
}

void HierarchicalFSMAgent::setUseStaticTransition(bool useStaticTransition) {
  HierarchicalFSMAgent::useStaticTransition = useStaticTransition;
}

double & HierarchicalFSMAgent::Q(const State &state, const string &machineState, int choice) {
  if (useStaticTransition && isStaticTransition(state, machineState, choice)) {
    double ret = 0.0;
    double sum = 0.0;

    for (auto &e : staticTransition[state][machineState][choice]) {
      for (auto &ee : e.second) {
        if (ee.first != machineState) {
          assert(numChoicesMap.count(ee.first));
          auto num_choices = numChoicesMap[ee.first];
          ret += ee.second * V(e.first, ee.first, num_choices);
          sum += ee.second;
        }
      }
    }

    if (sum > 0.0) {
      qtable_[state][machineState][choice] = ret / sum;
    }
  }

  return qtable_[state][machineState][choice];
}

void HierarchicalFSMAgent::saveStaticTransitions(string filename) {
//  std::ofstream ofs(filename);
//  boost::archive::xml_oarchive oa(ofs);
//  oa << BOOST_SERIALIZATION_NVP(numChoicesMap);
//  oa << BOOST_SERIALIZATION_NVP(staticTransitions);
}

void HierarchicalFSMAgent::loadStaticTransitions(string filename) {
//  std::ifstream ifs(filename);
//  if (!ifs.good())
//    return;
//  boost::archive::xml_iarchive ia(ifs);
//  ia >> BOOST_SERIALIZATION_NVP(numChoicesMap);
//  ia >> BOOST_SERIALIZATION_NVP(staticTransitions);
}
