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

    if (useStaticTransition) {
      saveStaticTransitions(name() + "_transitions.xml");

      dot::Graph G;
      for (auto &e : staticTransitions) {
        auto s = to_prettystring(e.first);
        boost::replace_all(s, ", {", ",\n{");
        G.addNode(s, "red");
        for (auto &o : e.second) {
          auto a = to_string(o.first);
          auto sa = s + ";  " + a;
          G.addNode(sa, "gray");
          G.addEdge(s, sa, "blue", a);
          for (auto &x : o.second) {
            auto s_ = to_prettystring(x.first);
            boost::replace_all(s_, ", {", ",\n{");
            auto n = to_string(x.second);
            G.addNode(s_, "orange");
            G.addEdge(sa, s_, "green", n);
          }
        }
      }

      G.dump(name() + "_transitions.dot");
    }
  }
}

void HierarchicalFSMAgent::step(Action a) {
  if (verbose) {
    cout << "Step " << steps << " | State: " << env_->get_state()
         << " x MachineState: " << getMachineState()
         << " | Action: " << action_name(Action(a));
  }

  double reward = env()->step(a);
  rewards += reward;
  steps += 1;

  accumulatedRewards += accumulatedDiscount * reward;
  accumulatedDiscount *= gamma;

  if (verbose) {
    cout << " | Reward: " << reward << endl;
  }
}

bool HierarchicalFSMAgent::running() {
  bool ret = !env()->terminate() && steps < max_steps;

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
  gamma = 0.95;
  steps = 0;
  accumulatedRewards = 0.0;
  accumulatedDiscount = 1.0;

  lastState = {0, 0, 0, 0};
  lastChoice = 0;
  lastChoiceTime = -1;
  stack.clear();
  lastMachineState.clear();

  staticTransitions.clear(); // dependent on input state
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
    staticTransitions[lastMachineState][lastChoice][machineState] += 1.0;

    if (hasCircle(staticTransitions)) {
      staticTransitions[lastMachineState][lastChoice].erase(
          machineState);
      if (staticTransitions[lastMachineState][lastChoice].empty()) {
        staticTransitions[lastMachineState].erase(lastChoice);
        if (staticTransitions[lastMachineState].empty()) {
          staticTransitions.erase(lastMachineState);
        }
      }
      assert(!hasCircle(staticTransitions));
    }
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
namespace {
bool dfs(
    const string &root,
    unordered_map<string,
        unordered_map<int, unordered_map<string, double>>> &G,
    unordered_set<string> &visited,
    unordered_set<string> &path) {
  if (path.count(root)) {
    return true;
  }

  visited.insert(root);
  path.insert(root);
  if (G.count(root)) {
    for (auto &pa : G[root]) {
      for (auto &next : pa.second) {
        if (dfs(next.first, G, visited, path))
          return true;
      }
    }
  }
  return false;
};
}

bool HierarchicalFSMAgent::hasCircle(unordered_map<string, \
    unordered_map<int, \
        unordered_map<string, double>>> &G)
{
  unordered_set<string> visited;
  for (auto &pa : G) {
    unordered_set<string> path;
    if (!visited.count(pa.first) && dfs(pa.first, G, visited, path))
      return true;
  }
  return false;
}

bool HierarchicalFSMAgent::isStaticTransition(const string &machine_state, int c) {
  return staticTransitions.count(machine_state) &&
         staticTransitions[machine_state].count(c) &&
         staticTransitions[machine_state][c].size();
}

bool HierarchicalFSMAgent::isUseStaticTransition() const {
  return useStaticTransition;
}

void HierarchicalFSMAgent::setUseStaticTransition(bool useStaticTransition) {
  HierarchicalFSMAgent::useStaticTransition = useStaticTransition;
}

double & HierarchicalFSMAgent::Q(const State &state, const string &machineState, int choice) {
  if (useStaticTransition && isStaticTransition(machineState, choice)) {
    double ret = 0.0;
    double sum = 0.0;
    for (auto &e : staticTransitions[machineState][choice]) {
      if (e.first != machineState) {
        assert(numChoicesMap.count(e.first));
        auto num_choices = numChoicesMap[e.first];
        ret += e.second * V(state, e.first, num_choices);
        sum += e.second;
      }
    }
    if (sum > 0.0) {
      qtable_[state][machineState][choice] = ret / sum;
    }
  }

  return qtable_[state][machineState][choice];
}

void HierarchicalFSMAgent::saveStaticTransitions(string filename) {
  std::ofstream ofs(filename);
  boost::archive::xml_oarchive oa(ofs);
  oa << BOOST_SERIALIZATION_NVP(numChoicesMap);
  oa << BOOST_SERIALIZATION_NVP(staticTransitions);
}

void HierarchicalFSMAgent::loadStaticTransitions(string filename) {
  std::ifstream ifs(filename);
  if (!ifs.good())
    return;
  boost::archive::xml_iarchive ia(ifs);
  ia >> BOOST_SERIALIZATION_NVP(numChoicesMap);
  ia >> BOOST_SERIALIZATION_NVP(staticTransitions);
}