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

void Root::run(const vector<string> & parameters) {
  Runner(get).operator()();
  if (running()) {
    Runner(put).operator()();
  }
  agent->Qupdate(state(), machineState(), 1);
}

Primitive::Primitive(HierarchicalFSMAgent *p, Action a, string name): HierarchicalFSM(p, "$" + name) {
  action = a;
}

Primitive::~Primitive() { }

void Primitive::run(const vector<string> & parameters) {
  Actuator(this, {to_prettystring(action)}).operator()(action);
}

Get::Get(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Get") {
  pickup = new Primitive(p, Pickup, action_name(Pickup));
  nav = new Navigate(p);

  choice = new ChoicePoint<HierarchicalFSM *>("@Get", {pickup, nav});
}

Get::~Get() {
  delete pickup;
  delete nav;

  delete choice;
}

void Get::run(const vector<string> & parameters) {
  while (running() && !agent->env()->loaded()) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    Runner(m, {to_string(state().passenger())}).operator()();
  }
}

Put::Put(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Put") {
  putdown = new Primitive(p, Putdown, action_name(Putdown));
  nav = new Navigate(p);

  choice = new ChoicePoint<HierarchicalFSM *>("@Put", {putdown, nav});
}

Put::~Put() {
  delete putdown;
  delete nav;

  delete choice;
}

void Put::run(const vector<string> & parameters) {
  while (running() && !agent->env()->unloaded()) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    Runner(m, {to_string(state().destination())}).operator()();
  }
}

Navigate::Navigate(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Nav") {
  east = new Primitive(p, East, action_name(East));
  south = new Primitive(p, South, action_name(South));
  west = new Primitive(p, West, action_name(West));
  north = new Primitive(p, North, action_name(North));

  choice = new ChoicePoint<HierarchicalFSM *>("@Nav", {east, south, west, north});
}

Navigate::~Navigate() {
  delete east;
  delete south;
  delete west;
  delete north;

  delete choice;
}

void Navigate::run(const vector<string> & parameters) {
  auto i = atoi(parameters[0].c_str());
  TaxiEnv::Position target = agent->env()->terminal(i);

  while (running() && agent->env()->taxi() != target) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    Runner(m).operator()();
  }
}

}
