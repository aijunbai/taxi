/*
 * MaxQOPAgent.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: baj
 */

#include "state.h"
#include "taxi.h"
#include "maxqop.h"
#include "uct.h"

#include <cassert>
#include <cstring>
#include <set>

#define DUMP_TREE 0

int MaxQOPAgent::Node::id = 0;

MaxQOPAgent::MaxQOPAgent(const bool test): OLAgent(test)
{
	maxq_[Root_T][Get_T] = new RootTransition(Get_T);
	maxq_[Root_T][Put_T] = new RootTransition(Put_T);

	maxq_[Get_T][Pickup_T] = new GetPutTransition(Get_T, Pickup_T);
	maxq_[Get_T][NavB_T] = new GetPutTransition(Get_T, NavB_T);
	maxq_[Get_T][NavG_T] = new GetPutTransition(Get_T, NavG_T);
	maxq_[Get_T][NavR_T] = new GetPutTransition(Get_T, NavR_T);
	maxq_[Get_T][NavY_T] = new GetPutTransition(Get_T, NavY_T);

	maxq_[Put_T][Putdown_T] = new GetPutTransition(Put_T, Putdown_T);
	maxq_[Put_T][NavB_T] = new GetPutTransition(Put_T, NavB_T);
	maxq_[Put_T][NavG_T] = new GetPutTransition(Put_T, NavG_T);
	maxq_[Put_T][NavR_T] = new GetPutTransition(Put_T, NavR_T);
	maxq_[Put_T][NavY_T] = new GetPutTransition(Put_T, NavY_T);

	maxq_[NavB_T][North_T] = new NavTransition(North_T);
	maxq_[NavB_T][South_T] = new NavTransition(South_T);
	maxq_[NavB_T][West_T] = new NavTransition(West_T);
	maxq_[NavB_T][East_T] = new NavTransition(East_T);

	maxq_[NavG_T][North_T] = new NavTransition(North_T);
	maxq_[NavG_T][South_T] = new NavTransition(South_T);
	maxq_[NavG_T][West_T] = new NavTransition(West_T);
	maxq_[NavG_T][East_T] = new NavTransition(East_T);

	maxq_[NavR_T][North_T] = new NavTransition(North_T);
	maxq_[NavR_T][South_T] = new NavTransition(South_T);
	maxq_[NavR_T][West_T] = new NavTransition(West_T);
	maxq_[NavR_T][East_T] = new NavTransition(East_T);

	maxq_[NavY_T][North_T] = new NavTransition(North_T);
	maxq_[NavY_T][South_T] = new NavTransition(South_T);
	maxq_[NavY_T][West_T] = new NavTransition(West_T);
	maxq_[NavY_T][East_T] = new NavTransition(East_T);

	for (int i = 0; i < TaskSize; ++i) {
		max_depth_[i] = 0;
	}

	max_depth_[Root_T] = 2;

	max_depth_[Get_T] = 2;
	max_depth_[Put_T] = 2;

	const int primitive_depth = 15;

	max_depth_[NavB_T] = primitive_depth;
	max_depth_[NavR_T] = primitive_depth;
	max_depth_[NavG_T] = primitive_depth;
	max_depth_[NavY_T] = primitive_depth;
}

MaxQOPAgent::~MaxQOPAgent()
{
	DumpSearchTree();
}

void MaxQOPAgent::DumpSearchTree()
{
	std::ofstream fout("search.dot");

	if (!fout.good()){
		perror("fopen");
		exit(1);
	}

	fout << search_tree_;
	fout.close();
}

bool MaxQOPAgent::IsPrimitive(Task task)
{
	return task == Pickup_T ||
			task == Putdown_T ||
			task == North_T ||
			task == South_T ||
			task == West_T ||
			task == East_T;
}

Action MaxQOPAgent::plan(const State & state)
{
	vector<int> depths(TaskSize, 0);
	ValuePrimitiveActionPair pair = EvaluateState(Root_T, state, depths);

#if DUMP_TREE
	DumpSearchTree();

	exit(0);
#endif

	return pair.primitive_action;
}

bool MaxQOPAgent::IsActiveState(Task task, const State & state)
{
	switch (task) {
	case Root_T: return true;

	case Get_T: return state.passenger() != int(TaxiEnv::EnvModel::ins().terminals().size());
	case Put_T: return state.passenger() == int(TaxiEnv::EnvModel::ins().terminals().size());

	case NavR_T:
	case NavY_T:
	case NavG_T:
	case NavB_T: return true;

	case Pickup_T:
	case Putdown_T:
	case North_T:
	case South_T:
	case East_T:
	case West_T: return true; //can performed in any state

	default: assert(0); return false;
	}
}

bool MaxQOPAgent::IsTerminalState(Task task, const State & state)
{
	switch (task) {
	case Root_T: return state.passenger() == state.destination();

	case Get_T: return state.passenger() == int(TaxiEnv::EnvModel::ins().terminals().size());
	case Put_T: return state.passenger() == state.destination();

	case NavR_T: return TaxiEnv::Position(state.x(), state.y()) == TaxiEnv::EnvModel::ins().terminals()[1];
	case NavY_T: return TaxiEnv::Position(state.x(), state.y()) == TaxiEnv::EnvModel::ins().terminals()[0];
	case NavG_T: return TaxiEnv::Position(state.x(), state.y()) == TaxiEnv::EnvModel::ins().terminals()[3];
	case NavB_T: return TaxiEnv::Position(state.x(), state.y()) == TaxiEnv::EnvModel::ins().terminals()[2];

	case Pickup_T:
	case Putdown_T:
	case North_T:
	case South_T:
	case East_T:
	case West_T: return true; //terminate in any state

	default: assert(0); return false;
	}
}

double MaxQOPAgent::TerminalEvaluation(Task task, const State & state)
{
	switch (task) {
	case Root_T:
		if (state.passenger() == int(TaxiEnv::EnvModel::ins().terminals().size())) { //passenger on taxi
			return -TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[state.destination()]) - 1 + 20; //taxi -> destination
		}
		else {
			return -TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[state.passenger()]) - 1
			- TaxiEnv::EnvModel::distance(TaxiEnv::EnvModel::ins().terminals()[state.passenger()], TaxiEnv::EnvModel::ins().terminals()[state.destination()]) - 1 + 20; //taxi -> passenger -> destination
		}

	case Get_T: return -TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[state.passenger()]) - 1; //taxi -> passenger
	case Put_T: return -TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[state.destination()]) - 1 + 20; //taxi -> destination

	case NavR_T: return -TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[1]);
	case NavY_T: return -TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[0]);
	case NavG_T: return -TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[3]);
	case NavB_T: return -TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[2]);

	case Pickup_T:
	case Putdown_T:
	case North_T:
	case South_T:
	case East_T:
	case West_T: return 0; //terminate in any state

	default: assert(0); return false;
	}
}

vector<pair<State, double> > MaxQOPAgent::TerminateStates(Task task, const State & state, Task action)
{
	return maxq_[task][action]->operator ()(state);
}

MaxQOPAgent::ValuePrimitiveActionPair MaxQOPAgent::ImmediateReward(const State & state, Task task)
{
	switch (task) {
	case Pickup_T:
		if (state.passenger() == int(TaxiEnv::EnvModel::ins().terminals().size()) || TaxiEnv::Position(state.x(), state.y()) !=
																																				TaxiEnv::EnvModel::ins().terminals()[state.passenger()]) {
			return ValuePrimitiveActionPair(-10, Pickup);
		}
		else {
			return ValuePrimitiveActionPair(-1, Pickup);
		}
	case Putdown_T:
		if (state.passenger() == int(TaxiEnv::EnvModel::ins().terminals().size()) && TaxiEnv::Position(state.x(), state.y()) ==
																																				TaxiEnv::EnvModel::ins().terminals()[state.destination()]) {
			return ValuePrimitiveActionPair(+20, Putdown);
		}
		else {
			return ValuePrimitiveActionPair(-10, Putdown);
		}
	case North_T: return ValuePrimitiveActionPair(-1, North);
	case South_T: return ValuePrimitiveActionPair(-1, South);
	case East_T: return ValuePrimitiveActionPair(-1, East);
	case West_T: return ValuePrimitiveActionPair(-1, West);

	default: assert(0); return ValuePrimitiveActionPair(-infty, Nil);
	}
}

MaxQOPAgent::ValuePrimitiveActionPair MaxQOPAgent::EvaluateState(Task task, const State & state, vector<int>& depths)
{
#if DUMP_TREE
	{
		Node node(task_name(task) + "\\n" +  state.str(), depths);

		if (!nodes_stack_.empty()) {
			search_tree_.addNode(node.str, nodes_stack_.top().color);
			search_tree_.addEdge(nodes_stack_.top().str, node.str, nodes_stack_.top().color);
		}
		else {
			search_tree_.addNode(node.str, "red");;
		}

		nodes_stack_.push(node);
	}
#endif

	if (IsPrimitive(task)) {
		return ImmediateReward(state, task);
	}
	else if (!IsActiveState(task, state) && !IsTerminalState(task, state)) {
		return ValuePrimitiveActionPair(-infty, Nil);
	}
	else if (IsTerminalState(task, state)) {
		return ValuePrimitiveActionPair(0, Nil);
	}
	else if (depths[task] >= max_depth_[task]) {
		return ValuePrimitiveActionPair(TerminalEvaluation(task, state), Nil);
	}
	else {
		if (get_prob() < 0.9 && cache_[task].count(RelevantStateTemplate(task, state)) && cache_[task][RelevantStateTemplate(task, state)].second <= depths[task]) {
			return cache_[task][RelevantStateTemplate(task, state)].first;
		}

		ValuePrimitiveActionPair ret(-infty, Nil);

		for (map<Task, TransitionFunc*>::iterator it = maxq_[task].begin(); it != maxq_[task].end(); ++it) {
			if (IsPrimitive(it->first) || !IsTerminalState(it->first, state)) {
				ValuePrimitiveActionPair tmp = EvaluateState(it->first, state, depths);
				tmp.value += EvaluateCompletion(task, state, it->first, depths);

#if DUMP_TREE
				nodes_stack_.pop();
#endif

				if (tmp.value > ret.value) {
					ret = tmp;
				}
			}
		}

		//update cache
		if (!cache_[task].count(RelevantStateTemplate(task, state)) || depths[task] <= cache_[task][RelevantStateTemplate(task, state)].second) {
			cache_[task][RelevantStateTemplate(task, state)] = std::make_pair(ret, depths[task]);
		}

		if (ret.primitive_action == Nil) {
			assert(0);
		}

		return ret;
	}
}

double MaxQOPAgent::EvaluateCompletion(Task task, const State & state, Task action, vector<int>& depths)
{
	vector<pair<State, double> > samples = TerminateStates(task, state, action);
	double value = 0.0;

	for (int i = 0; i < int(samples.size()); ++i) {

		depths[task] += 1;
#if DUMP_TREE
		nodes_stack_.top().color = "red";
#endif

		ValuePrimitiveActionPair tmp = EvaluateState(task, samples[i].first, depths);

#if DUMP_TREE
		nodes_stack_.pop();
#endif
		depths[task] -= 1;

		value += tmp.value * samples[i].second;
	}

	return value;
}

State MaxQOPAgent::RelevantStateTemplate(Task task, const State& state) //state abstractions for non-primitive actions
{
	switch (task) {
	case Root_T: return state;

	case Get_T:
	{
		State tmp(state);
		tmp.destination() = 0; //irrelevant
		return tmp;
	}
	case Put_T:
	{
		State tmp(state);
		tmp.passenger() = 0;
		return tmp;
	}

	case NavR_T:
	case NavY_T:
	case NavG_T:
	case NavB_T:
	{
		State tmp(state);
		tmp.passenger() = 0;
		tmp.destination() = 0;
		return tmp;
	}

	default: assert(0); return state;
	}
}
