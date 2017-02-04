//
// Created by baj on 1/14/17.
//

#include "HierarchicalFSM.h"

#include "HierarchicalFSM.h"
#include "ChoicePoint.h"
#include "prettyprint.h"

using namespace std;

namespace std {
const string &to_string(fsm::HierarchicalFSM *m) { return m->getName(); }
}

namespace fsm {

HierarchicalFSM::HierarchicalFSM(HierarchicalFSMAgent *p, const std::string &name)
    : agent(p), name(name) {
}

HierarchicalFSM::~HierarchicalFSM() { }

void HierarchicalFSM::action(Action a) {
  agent->step(a);
}

const string &HierarchicalFSM::getName() const { return name; }

bool HierarchicalFSM::running() {
  return agent->running();
}

string HierarchicalFSM::getStackStr() {
  string str;
  for (auto &s : agent->getMachineState())
    str += s + " ";
  return str;
}

Root::Root(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Root") {
  get = new Get(p);
  put = new Put(p);
  refuel = new Refuel(p);

  choice = new ChoicePoint<HierarchicalFSM *>("@Root", {get, put, refuel});
}

Root::~Root() {
  delete get;
  delete put;
  delete refuel;

  delete choice;
}

void Root::run(unordered_map<string, int> parameters) {
  while (running()) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    Runner(m).operator()();
  }

  agent->Qupdate(state(), machineState(), 1, agent->steps);

  if (agent->history.size() > 100) {
    agent->showHistory();
    exit(1);
  }
}

Primitive::Primitive(HierarchicalFSMAgent *p, Action a, string name): HierarchicalFSM(p, "$" + name) {
  action = a;
}

Primitive::~Primitive() {
}

void Primitive::run(unordered_map<string, int> parameters) {
  Actuator(this, {to_prettystring(action)}).operator()(action);
}

Get::Get(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Get") {
  pickup = new Primitive(p, Pickup, action_name(Pickup));
  nav = new Navigate(p);

#if UNDETERMINISTIC_MACHINE
  choice = new ChoicePoint<HierarchicalFSM *>("@Get", {pickup, nav});
#endif
}

Get::~Get() {
  delete pickup;
  delete nav;

#if UNDETERMINISTIC_MACHINE
  delete choice;
#endif
}

void Get::run(unordered_map<string, int> parameters) {
#if UNDETERMINISTIC_MACHINE
  while (running() && !state().loaded()) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    if (m == nav) {
      Runner(m, {{"target", state().passenger()}}).operator()();
    }
    else {
      Runner(m).operator()();
    }
  }
#else
  if (running() && !state().loaded()) {
    Runner(nav, {{"target", state().passenger()}}).operator()();
    if (running()) Runner(pickup).operator()();
  }
#endif
}

Put::Put(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Put") {
  putdown = new Primitive(p, Putdown, action_name(Putdown));
  nav = new Navigate(p);

#if UNDETERMINISTIC_MACHINE
  choice = new ChoicePoint<HierarchicalFSM *>("@Put", {putdown, nav});
#endif
}

Put::~Put() {
  delete putdown;
  delete nav;

#if UNDETERMINISTIC_MACHINE
  delete choice;
#endif
}

void Put::run(unordered_map<string, int> parameters) {
  if (!state().loaded()) return;

#if UNDETERMINISTIC_MACHINE
  while (running() && !state().unloaded()) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    if (m == nav) {
      Runner(m, {{"target", state().destination()}}).operator()();
    }
    else {
      Runner(m).operator()();
    }
  }
#else
  if (running() && !state().unloaded()) {
    Runner(nav, {{"target", state().destination()}}).operator()();
    if (running()) Runner(putdown).operator()();
  }
#endif
}

Refuel::Refuel(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Refuel") {
  fillup = new Primitive(p, Fillup, action_name(Fillup));
  nav = new Navigate(p);

#if UNDETERMINISTIC_MACHINE
  choice = new ChoicePoint<HierarchicalFSM *>("@Refuel", {fillup, nav});
#endif
}

Refuel::~Refuel() {
  delete fillup;
  delete nav;

#if UNDETERMINISTIC_MACHINE
  delete choice;
#endif
}

void Refuel::run(unordered_map<string, int> parameters) {
#if UNDETERMINISTIC_MACHINE
  while (running() && !state().refueled()) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    if (m == nav) {
      Runner(m, {{"target", 4}}).operator()();
    }
    else {
      Runner(m).operator()();
    }
  }
#else
  if (running() && !state().refueled()) {
    Runner(nav, {{"target", 4}}).operator()();
    if (running()) Runner(fillup).operator()();
  }
#endif
}

Navigate::Navigate(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Nav") {
  north = new Primitive(p, North, action_name(North));
  south = new Primitive(p, South, action_name(South));
  east = new Primitive(p, East, action_name(East));
  west = new Primitive(p, West, action_name(West));

  choice = new ChoicePoint<HierarchicalFSM *>("@Dir", {north, south, east, west});
}

Navigate::~Navigate() {
  delete east;
  delete south;
  delete west;
  delete north;

  delete choice;
}

void Navigate::run(unordered_map<string, int> parameters) {
  int t = parameters["target"];
  Position target;
  if (t >= 0 && t <= 3) {
    target = agent->env()->terminal(t);
  }
  else if (t == 4) {
    target = TaxiEnv::Model::ins().fuelPosition();
  }
  else {
    return;
  }

  while (running() && state().taxiPosition() != target) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    Runner(m).operator()();
  }
}

}
