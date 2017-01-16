/*
 * astar.cpp
 *
 *  Created on: Oct 25, 2011
 *      Author: baj
 */

#include "astar.h"

#include <algorithm>

#define DUMP_TREE 0


AStarAgent::AStarAgent(const bool test): OLAgent(test) {
	// TODO Auto-generated constructor stub

}

AStarAgent::~AStarAgent() {
	// TODO Auto-generated destructor stub
}

Action AStarAgent::plan(const State & state)
{
	Action action = astar(state);

#if DUMP_TREE
	DumpSearchTree();
	exit(0);
#endif

	for (uint i = 0; i < nodes_pool_.size(); ++i) {
		delete nodes_pool_[i];
	}
	nodes_pool_.clear();

	return action;
}

std::pair<State, double> AStarAgent::apply(State state, const Action action)
{
	TaxiEnv::Position taxi(state.x(), state.y());
	double cost = 0.0;

	switch (action) {
	case North:
	case South:
	case East:
	case West:
		taxi = taxi + TaxiEnv::model->delta_[taxi.x][taxi.y][action];
		state.x() = taxi.x;
		state.y() = taxi.y;
		cost = 1.0;
		break;
	case Pickup:
		if (state.passenger() != int(TaxiEnv::model->terminals().size()) && taxi == TaxiEnv::model->terminals()[state.passenger()]) {
			state.passenger() = int(TaxiEnv::model->terminals().size());
			cost = 1.0;
		}
		else {
			cost = 10.0;
		}
		break;
	case Putdown:
		if (state.passenger() == int(TaxiEnv::model->terminals().size()) && taxi == TaxiEnv::model->terminals()[state.destination()]) {
			state.passenger() = state.destination();
			cost = /*20.0*/0.0;
		}
		else {
			cost = 10.0;
		}
		break;
	default:
		break;
	}

	return std::make_pair(state, cost);
}

Action AStarAgent::astar(const State & start)
{
	if (cache_.count(start)) {
		return cache_[start];
	}

	std::priority_queue<Node*, std::vector<Node*>, NodePtrCompair> openqueue;
	std::set<State> closedset;

	openqueue.push(create_node(start, 0, 0, Nil));

	while (!openqueue.empty()) {
		Node *node = openqueue.top();
		openqueue.pop();
		closedset.insert(node->state);

#if DUMP_TREE
		search_tree_.addNode(node->name(), "green");
#endif

		if (node->state.passenger() == node->state.destination() /*|| closedset_.size() > 1*/) { //goal
#if DUMP_TREE
			search_tree_.addNode(node->name(), "red");
#endif

			if (node->parent) {
				while (node->parent->parent) {
					cache_[node->parent->state] = node->action_from;

					node = node->parent;
#if DUMP_TREE
					search_tree_.addNode(node->parent->name(), "red");
					search_tree_.addNode(node->name(), "red");
					search_tree_.addEdge(node->parent->name(), node->name(), "red", action_name(node->action_from));
#endif
				}
			}

			if (node->parent) {
				cache_[node->parent->state] = node->action_from;
			}

			return node->action_from;
		}

		for (int action = 0; action < ActionSize; ++action) {
			std::pair<State, double> state_cost = apply(node->state, Action(action));

			Node *child_node = create_node(state_cost.first, node->g + state_cost.second, node, Action(action));
#if DUMP_TREE
			search_tree_.addEdge(node->name(), child_node->name(), "", action_name(Action(action)));
#endif

			if (!closedset.count(child_node->state)) {
				openqueue.push(child_node);
			}
		}
	}

	return Nil;
}
