/*
 * MaxQOPAgent.h
 *
 *  Created on: Oct 10, 2011
 *      Author: baj
 */

#ifndef MAXQOLAGENT_H_
#define MAXQOLAGENT_H_

#include "olagent.h"
#include "table.h"
#include "dot_graph.h"

#include <vector>
#include <map>
#include <cassert>
#include <stack>

using namespace std;

class MaxQOPAgent: public OLAgent {
public:
	MaxQOPAgent(const bool test);
	virtual ~MaxQOPAgent();

	Action plan(const State & state);

private:
	enum Task { //defined subtask for MAXQ hierarchical structure
		Root_T = 0,

				Get_T,
				Put_T,

				NavR_T,
				NavY_T,
				NavB_T,
				NavG_T,

				Pickup_T,
				Putdown_T,
				North_T,
				South_T,
				East_T,
				West_T,

				TaskSize
	};

	static std::string task_name(Task task)
	{
		switch (task) {
		case Root_T: return "Root";

		case Get_T: return "Get";
		case Put_T: return "Put";

		case NavR_T: return "NavR";
		case NavY_T: return "NavY";
		case NavG_T: return "NavG";
		case NavB_T: return "NavB";

		case Pickup_T: return "Pickup";
		case Putdown_T: return "Putdown";
		case North_T: return "North";
		case South_T: return "South";
		case East_T: return "East";
		case West_T:  return "West";

		default: assert(0);  return "Nil";
		}
	}

	struct ValuePrimitiveActionPair {
		double value;
		Action primitive_action;

		explicit ValuePrimitiveActionPair(double v = -infty, Action a = Nil): value(v), primitive_action(a) { }
	};

	static const int infty = 1e6;

	ValuePrimitiveActionPair EvaluateState(Task task, const State & state, vector<int>& depths);

	double EvaluateCompletion(Task task, const State & state, Task action, vector<int>& depths);

	bool IsActiveState(Task task, const State & state);
	bool IsTerminalState(Task task, const State & state);

	bool IsPrimitive(Task task);

	ValuePrimitiveActionPair ImmediateReward(const State & state, Task task);

	double TerminalEvaluation(Task task, const State & state);

	vector<pair<State, double> > TerminateStates(Task task, const State & state, Task action);

private:
	struct TransitionFunc {
		virtual vector<pair<State, double> > operator()(const State& state) = 0;

		TransitionFunc(Task action): action_(action) { }
		Task action_;
	};

	struct RootTransition: public TransitionFunc {
		RootTransition(Task action): TransitionFunc(action) { }

		vector<pair<State, double> > operator()(const State& state) {
			vector<pair<State, double> > samples;

			State tmp(state);

			if (action_ == Get_T) {
				tmp.x() = TaxiEnv::EnvModel::ins().terminals()[state.passenger()].x;
				tmp.y() = TaxiEnv::EnvModel::ins().terminals()[state.passenger()].y;
				tmp.passenger() = TaxiEnv::EnvModel::ins().terminals().size();
			}
			else if (action_ == Put_T) {
				tmp.x() = TaxiEnv::EnvModel::ins().terminals()[state.destination()].x;
				tmp.y() = TaxiEnv::EnvModel::ins().terminals()[state.destination()].y;
				tmp.passenger() = state.destination();
			}
			else {
				assert(0);
			}

			samples.push_back(make_pair(tmp, 1.0));

			return samples;
		}
	};

	struct GetPutTransition: public TransitionFunc {
		GetPutTransition(Task task, Task action): TransitionFunc(action), task_(task) {

		}

		vector<pair<State, double> > operator()(const State& state) {
			vector<pair<State, double> > samples;

			State tmp(state);

			switch (action_) {
			case Pickup_T:
				if (task_ == Get_T) {
					if (TaxiEnv::Position(state.x(), state.y()) == TaxiEnv::EnvModel::ins().terminals()[state.passenger()]) {
						tmp.passenger() = TaxiEnv::EnvModel::ins().terminals().size();
					}
				}
				else {
					assert(0);
				}
				break;

			case Putdown_T:
				if (task_ == Put_T) {
					if (state.passenger() == int(TaxiEnv::EnvModel::ins().terminals().size()) && TaxiEnv::Position(state.x(), state.y()) ==
                                                                              TaxiEnv::EnvModel::ins().terminals()[state.destination()]) {
						tmp.passenger() = state.destination();
					}
				}
				else {
					assert(0);
				}
				break;

			case NavB_T:
				tmp.x() = TaxiEnv::EnvModel::ins().terminals()[2].x;
				tmp.y() = TaxiEnv::EnvModel::ins().terminals()[2].y;
				break;

			case NavY_T:
				tmp.x() = TaxiEnv::EnvModel::ins().terminals()[0].x;
				tmp.y() = TaxiEnv::EnvModel::ins().terminals()[0].y;
				break;

			case NavR_T:
				tmp.x() = TaxiEnv::EnvModel::ins().terminals()[1].x;
				tmp.y() = TaxiEnv::EnvModel::ins().terminals()[1].y;
				break;

			case NavG_T:
				tmp.x() = TaxiEnv::EnvModel::ins().terminals()[3].x;
				tmp.y() = TaxiEnv::EnvModel::ins().terminals()[3].y;
				break;

			default: assert(0); break;
			}

			samples.push_back(make_pair(tmp, 1.0));
			return samples;
		}

		Task task_;
	};

	static Action translate_primitive(Task action) {
		switch (action) {
		case North_T: return North;
		case South_T: return South;
		case East_T: return East;
		case West_T: return West;
		default: assert(0); break;
		}

		assert(0);
		return Nil;
	}

	static Action sample_primitive(Task action) {
		double prob = get_prob();

		if (prob <= 0.1) {
			switch (action) {
			case North_T: return West;
			case South_T: return East;
			case East_T: return North;
			case West_T: return South;
			default: assert(0); break;
			}
		}
		else if (prob <= 0.2) {
			switch (action) {
			case North_T: return East;
			case South_T: return West;
			case East_T: return South;
			case West_T: return North;
			default: assert(0); break;
			}
		}

		return translate_primitive(action);
	}

	struct NavTransition: public TransitionFunc {
		NavTransition(Task action): TransitionFunc(action){ }

		vector<pair<State, double> > operator()(const State& state) {
			vector<pair<State, double> > samples;

#if 1
			vector<Action> actions(3, translate_primitive(action_));
			vector<double> distribution(3, 0.1);
			vector<TaxiEnv::Position> locations(3, TaxiEnv::Position(state.x(), state.y()));

			distribution[0] = 0.8;

			switch (action_) {
			case North_T: actions[1] = East; actions[2] = West; break;
			case South_T: actions[1] = East; actions[2] = West; break;
			case East_T: actions[1] = South; actions[2] = North; break;
			case West_T: actions[1] = South; actions[2] = North; break;
			default: assert(0); break;
			}

			State tmp(state);

			for (int i = 0; i < 3; ++i) {
				locations[i] = locations[i] + TaxiEnv::EnvModel::ins().delta_[locations[i].x][locations[i].y][actions[i]];

				tmp.x() = locations[i].x;
				tmp.y() = locations[i].y;

				samples.push_back(make_pair(tmp, distribution[i]));
			}
#else
			Action action = translate_primitive(action_); //maximal likelihood action
			TaxiEnv::Position location(state.x(), state.y());

			location = location + TaxiEnv::EnvModel::ins().delta_[location.x][location.y][action];

			State tmp(state);

			tmp.x() = location.x;
			tmp.y() = location.y;

			samples.push_back(make_pair(tmp, 1.0));
#endif

			return samples;
		}
	};

	State RelevantStateTemplate(Task task, const State& state);

	private:
	map<Task, map<Task, TransitionFunc*> > maxq_;
	int max_depth_[TaskSize];

	HashMap<State, std::pair<ValuePrimitiveActionPair, int> > cache_[TaskSize];

	private:
	void DumpSearchTree();

	dot::Graph search_tree_;

	struct Node {
		static int id;

		std::string str;
		std::string color;

		Node(const std::string& info, const vector<int>& depths) {
			color = "green";

			std::stringstream ss;
			ss << id++;

			str += "#" + ss.str() + ":\\n" + info + "\\n[";

			for (uint i = 0; i < depths.size(); ++i) {
				std::stringstream ss;
				ss << depths[i];
				str += ss.str();

				if (i != depths.size() - 1) {
					str += " ";
				}
			}
			str += "]";
		}
	};

	std::stack<Node> nodes_stack_;
};

#endif /* MAXQOLAGENT_H_ */
