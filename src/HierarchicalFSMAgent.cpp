//
// Created by baj on 1/14/17.
//

#include "HierarchicalFSMAgent.h"
#include "HierarchicalFSM.h"
#include "ChoicePoint.h"

using namespace fsm;

HierarchicalFSMAgent::HierarchicalFSMAgent(const bool test): HierarchicalAgent(test) {
  max_steps = 1024;
  verbose = false;

  reset();
}

HierarchicalFSMAgent::~HierarchicalFSMAgent() {
  if (!test()) {
    if (leverageInternalTransitions()) {
      for (auto &o : internalTransitions) {
        dot::Graph G;
        for (auto &e : o.second) {
          auto s = to_prettystring(e.first);
          boost::replace_all(s, ", {", ",\n{");
          G.addNode(s, "red");
          for (auto &o : e.second) {
            auto a = to_prettystring(o.first);
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

        G.dump(name() + "_" + to_string(o.first) + "_transitions.dot");
      }
    }
  }
}

void HierarchicalFSMAgent::step(Action a) {
  auto state = env()->state();
  auto machineState = getMachineState();

  if (verbose) {
    cout << "Step " << steps << " | State: " << state
         << " x MachineState: " << machineState
         << " | Action: " << action_name(Action(a));
  }

  double reward = env()->step(a);
  inc(reward);

  accumulatedRewards += accumulatedDiscount * reward;
  accumulatedDiscount *= gamma;

  if (verbose) {
    cout << " | Reward: " << reward << endl;
  }

//  history.push_back(make_tuple(state, machineState, a, reward));
}

//void HierarchicalFSMAgent::showHistory()
//{
//  int t = 0;
//  for (auto &e : history) {
//    cout << "Step: " << t++ << " {{\n" << e << "\n}}\n" << endl;
//  }
//}

bool HierarchicalFSMAgent::running() {
  bool ret = !env()->state().terminated() && steps < max_steps;
  return ret;
}

void HierarchicalFSMAgent::reset() {
  HierarchicalAgent::reset();

  accumulatedRewards = 0.0;
  accumulatedDiscount = 1.0;
  lastState = {0, 0, 0, 0};
  lastChoice = 0;
  lastChoiceTime = -1;
  stack.clear();
  lastMachineState.clear();
//  history.clear();
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

int HierarchicalFSMAgent::selectChoice(const State &state, const string &machineState, int numChoices) {
  std::vector<double> distri(numChoices);
  for (int a = 0; a < numChoices; ++a) {
    distri[a] = Q(state, machineState, a);
  }

  return PolicyFactory::instance().CreatePolicy(test()? PT_Greedy: PT_EpsilonGreedy)->get_action(distri);
}

int HierarchicalFSMAgent::argmaxQ(const State &state, const string &machineState, int numChoices) {
  std::vector<double> distri(numChoices);
  for (int a = 0; a < numChoices; ++a) {
    distri[a] = Q(state, machineState, a);
  }

  return PolicyFactory::instance().CreatePolicy(PT_Greedy)->get_action(distri);
}

double HierarchicalFSMAgent::V(const State &state, const string &machineState, int numChoices)
{
  if (state.terminated()) return 0.0;
  return Q(state, machineState, argmaxQ(state, machineState, numChoices));
}

int HierarchicalFSMAgent::QUpdate(
    const State &state, const string &machineState, int numChoices, int current_time)
{
  assert(!numChoicesMap.count(machineState) || numChoicesMap[machineState] == numChoices);
  numChoicesMap[machineState] = numChoices;

  if (leverageInternalTransitions() &&
      current_time == lastChoiceTime && lastMachineState.size() &&
      machineState.size() && lastMachineState != machineState) {
    auto cond = env()->stateConditions(lastState);
    assert(lastState == state);

//    std::vector<std::string> x = split(machineState, ',');
//
//    auto t = x[4][8];
//    if (x[2][3] == 'G') {
//      assert(t == '4' || t == '0' + get<0>(cond));
//    }
//    else if (x[2][3] == 'P') {
//      assert(t == '0' + get<1>(cond));
//    }
//    else {
//      assert(t == 'F');
//    }

    internalTransitions[cond][lastMachineState][lastChoice][machineState] += 1.0;
    assert(internalTransitions[cond][lastMachineState][lastChoice].size() == 1);
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
  auto cond = env()->stateConditions(state);
  return internalTransitions[cond].count(machine_state) &&
         internalTransitions[cond][machine_state].count(c) &&
         internalTransitions[cond][machine_state][c].size();
}

double & HierarchicalFSMAgent::Q(const State &state, const string &machineState, int choice) {
  if (leverageInternalTransitions() && isStaticTransition(state, machineState, choice)) {
    double ret = 0.0;
    double sum = 0.0;

    auto cond = env()->stateConditions(state);
    for (auto &e : internalTransitions[cond][machineState][choice]) {
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

