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

  choice = new ChoicePoint<HierarchicalFSM *>("@RootGet", {get, put, refuel});
}

Root::~Root() {
  delete get;
  delete put;
  delete refuel;

  delete choice;
}

void Root::run(const vector<string> & parameters) {
  while (running()) {
    MakeChoice<HierarchicalFSM *> c(this, choice);
    auto m = c();
    Runner(m).operator()();
  }

  agent->Qupdate(state(), machineState(), 1, agent->steps);
}

Primitive::Primitive(HierarchicalFSMAgent *p, Action a, string name): HierarchicalFSM(p, "$" + name) {
  action = a;
}

Primitive::~Primitive() {
}

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
  if (running()) {
    Runner(nav, {to_string(state().passenger())}).operator()();
    if (running()) {
      Runner(pickup).operator()();
    }
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
  if (running()) {
    Runner(nav, {to_string(state().destination())}).operator()();
    if (running()) {
      Runner(putdown).operator()();
    }
  }
}

Refuel::Refuel(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Refuel") {
  fillup = new Primitive(p, Fillup, action_name(Fillup));
  nav = new Navigate(p);

  choice = new ChoicePoint<HierarchicalFSM *>("@Refuel", {fillup, nav});
}

Refuel::~Refuel() {
  delete fillup;
  delete nav;

  delete choice;
}

void Refuel::run(const vector<string> & parameters) {
  if (running()) {
    Runner(nav, {"F"}).operator()();
    if (running()) {
      Runner(fillup).operator()();
    }
  }
}

Navigate::Navigate(HierarchicalFSMAgent *p): HierarchicalFSM(p, "$Nav") {
  north = new Primitive(p, North, action_name(North));
  south = new Primitive(p, South, action_name(South));
  east = new Primitive(p, East, action_name(East));
  west = new Primitive(p, West, action_name(West));

  dir_choice = new ChoicePoint<HierarchicalFSM *>("@Dir", {north, south, east, west});
  step_choice = new ChoicePoint<int>("@Step", {1, 2, 3, 4});
}

Navigate::~Navigate() {
  delete east;
  delete south;
  delete west;
  delete north;

  delete dir_choice;
  delete step_choice;
}

void Navigate::run(const vector<string> & parameters) {
  char c = parameters[0][0];
  Position target = c == 'F'? TaxiEnv::Model::ins().fuelPosition() : agent->env()->terminal(c - '0');

  while (running() && agent->env()->state().taxiPosition() != target) {
    MakeChoice<HierarchicalFSM *> c(this, dir_choice);
    auto m = c();

    {
      MakeChoice<int> c(this, step_choice);
      auto n = c();

      for (int i = 0; i < n && running() && agent->env()->state().taxiPosition() != target; ++i) {
        Runner(m).operator()();
      }
    }
  }
}

}
