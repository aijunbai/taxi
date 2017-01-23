/*
 * astar.h
 *
 *  Created on: Oct 25, 2011
 *      Author: baj
 */

#ifndef ASTAR_H_
#define ASTAR_H_

#include "olagent.h"
#include "state.h"
#include "taxi.h"
#include "dot_graph.h"
#include "table.h"

#include <queue>
#include <vector>
#include <set>
#include <fstream>


class AStarAgent: public OLAgent {
public:
	AStarAgent(const bool test);
	virtual ~AStarAgent();

	Action plan(const State & state);

private:
	struct Node {
		State state;
		double g;
		double h;
		Node *parent;
		Action action_from;

		Node(const State& state, double g, Node *parent, Action action_from): state(state), g(g), h(0.0), parent(parent), action_from(action_from) {
			if (state.passenger() == int(TaxiEnv::EnvModel::ins().terminals().size())) { //on taxi
				h = TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[state.destination()]) + 1; //taxi -> destination
			}
			else { //not on taxi
				h = TaxiEnv::EnvModel::distance(TaxiEnv::Position(state.x(), state.y()), TaxiEnv::EnvModel::ins().terminals()[state.passenger()]) + 1
					+ TaxiEnv::EnvModel::distance(TaxiEnv::EnvModel::ins().terminals()[state.passenger()], TaxiEnv::EnvModel::ins().terminals()[state.destination()]) + 1; //taxi -> passenger -> destination
			}
		}

		bool operator<(const Node& o) const {
			return g + h > o.g + o.h;
		}

		std::string name() {
			return state.str();
		}
	};

	struct NodePtrCompair {
		bool operator()(const Node *a, const Node *b) {
			return *a < *b;
		}
	};

private:
	Action astar(const State & start);

	std::pair<State, double> apply(State state, const Action action);

	Node *create_node(const State& state, double g, Node *parent, Action action_from) {
		Node *node = new Node(state, g, parent, action_from);

		nodes_pool_.push_back(node);

		return node;
	}

	std::vector<Node*> nodes_pool_;
	HashMap<State, Action> cache_;

	void DumpSearchTree() {
		std::ofstream fout("search.dot");

		if (!fout.good()){
			perror("fopen");
			exit(1);
		}

		fout << search_tree_;
		fout.close();
	}

	dot::Graph search_tree_;
};

#endif /* ASTAR_H_ */
