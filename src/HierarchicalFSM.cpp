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
}

Root::~Root() {
  delete get;
  delete put;
}

void Root::run(unordered_map<string, int> parameters) {
  Runner(get).operator()();
  if (running()) Runner(put).operator()();

  agent->Qupdate(state(), machineState(), 1, agent->steps);
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
}

Get::~Get() {
  delete pickup;
  delete nav;
}

void Get::run(unordered_map<string, int> parameters) {
  if (running() && !state().loaded()) {
    Runner(nav, {{"target", state().passenger()}}).operator()();
    if (running()) Runner(pickup).operator()();
  }
}

Put::Put(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Put") {
  putdown = new Primitive(p, Putdown, action_name(Putdown));
  nav = new Navigate(p);
}

Put::~Put() {
  delete putdown;
  delete nav;
}

void Put::run(unordered_map<string, int> parameters) {
  if (!state().loaded()) return;

  if (running() && !state().unloaded()) {
    Runner(nav, {{"target", state().destination()}}).operator()();
    if (running()) Runner(putdown).operator()();
  }
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
